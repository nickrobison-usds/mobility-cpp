//
// Created by Nicholas Robison on 7/27/20.
//


#include "config.cpp"
#include "SafegraphCBGMapper.hpp"
#include "SafegraphCBGTiler.hpp"
#include <io/parquet.hpp>
#include <hpx/program_options.hpp>
#include <hpx/hpx_init.hpp>
#include <map-tile/MapTileBuilder.hpp>
#include <map-tile/client/MapTileClient.hpp>
#include <map-tile/coordinates/Coordinate3D.hpp>
#include <map-tile/io/FileProvider.hpp>
#include <shared/constants.hpp>
#include <shared/DateUtils.hpp>
#include <shared/DirectoryUtils.hpp>
#include <shared/data.hpp>
#include <shared/HostnameLogger.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/pattern_formatter.h>

using namespace std;

// Register the map-tile instance
typedef vector<cbg_centrality> reduce_type;
REGISTER_MAPPER(v2, mt::coordinates::Coordinate3D, SafegraphCBGMapper, SafegraphCBGTiler, reduce_type, string,
                mt::io::FileProvider);

int hpx_main(hpx::program_options::variables_map &vm) {
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<shared::HostnameLogger>('h').set_pattern("[%l] [%h] [%H:%M:%S %z] [thread %t] %v");
    spdlog::set_formatter(std::move(formatter));

    if (!vm.count("silent")) {
        spdlog::set_level(spdlog::level::debug);
    }

    const auto config_path = vm["config"].as<string>();
    const auto config = YAML::LoadFile(config_path).as<CentralityConfig>();

    // Build the reference file paths
    const auto cbg_path = shared::DirectoryUtils::build_path(config.data_dir, config.cbg_shp);
    const auto poi_path = shared::DirectoryUtils::build_path(config.data_dir, config.poi_parquet);
    const auto output_path = shared::DirectoryUtils::build_path(config.data_dir, config.output_dir);
    const auto log_path = shared::DirectoryUtils::build_path(config.data_dir, config.log_dir);

    if (!fs::exists(output_path)) {
        spdlog::debug("Creating output directory");
        fs::create_directory(output_path);
    }

    const auto locales = hpx::find_all_localities();
    using namespace mt::coordinates;
    // Compute the Z-index, the number of days in the analysis
    const auto time_bounds = chrono::duration_cast<shared::days>(config.end_date - config.start_date).count();
    std::array<std::size_t, 3> stride{static_cast<std::size_t>(time_bounds),
                                      static_cast<std::size_t>(floor(shared::MAX_CBG / locales.size())), shared::MAX_CBG};

    const auto sd = chrono::floor<date::days>(config.start_date);
    const auto ed = chrono::floor<date::days>(config.end_date);

    std::map<std::string, std::string> config_values;
    config_values["poi_path"] = poi_path.string();
    config_values["cbg_path"] = cbg_path.string();
    config_values["start_date"] = std::to_string(sd.time_since_epoch().count());
    config_values["end_date"] = std::to_string(ed.time_since_epoch().count());
    config_values["output_dir"] = output_path.string();
    config_values["output_name"] = config.output_name;
    config_values["log_dir"] = log_path.string();

    // Partition the input files, try one for each tile
    const auto csv_path = shared::DirectoryUtils::build_path(config.data_dir, config.patterns_csv);

    // Create the client using the builder
    mt::MapTileBuilder<v2, Coordinate3D, SafegraphCBGMapper, SafegraphCBGTiler, reduce_type> builder(Coordinate3D(0, 0, 0),
                                                                                                     Coordinate3D(
                                                                                                          time_bounds,
                                                                                                          shared::MAX_CBG,
                                                                                                          shared::MAX_CBG),
                                                                                                     csv_path);

    auto engine = builder
            .set_stride(stride)
            .set_regex(".*patterns\\.csv")
            .set_config_values(config_values)
            .build();

    // initialize
    vector<hpx::future<void>> init_results = engine.initialize();
    hpx::wait_all(init_results);

    // tile
    vector<hpx::future<void>> results = engine.tile();
    hpx::wait_all(results);
    spdlog::info("Tile complete, beginning computation");

    // Now, compute
    vector<hpx::future<void>> compute_results = engine.compute();
    hpx::wait_all(compute_results);
    spdlog::info("Computing completed");

    // And, reduce
    vector<hpx::future<reduce_type>> reduce_results = engine.reduce();
    hpx::wait_all(reduce_results);

    std::vector<cbg_centrality> unwrapped;
    for (auto &f : reduce_results) {
        const auto v = f.get();
        unwrapped.reserve(unwrapped.size() + v.size());
        std::move(v.begin(), v.end(),
                  std::back_inserter(unwrapped));
    }
    spdlog::info("Reducing completed");

    // Write out the CBGs by rank
    const auto rank_file = shared::DirectoryUtils::build_path(output_path, "cbg-ranks.parquet");
    const io::Parquet p(rank_file.string());
    arrow::StringBuilder _cbg_builder;
    arrow::Date32Builder _date_builder;
    arrow::DoubleBuilder _rank_builder;

    arrow::Status status;
    for (const auto &rp : unwrapped) {
        status = _cbg_builder.Append(rp.cbg);
        status = _date_builder.Append(rp.date.time_since_epoch().count());
        status = _rank_builder.Append(rp.value);
    }

    std::shared_ptr<arrow::Array> cbg_array;
    status = _cbg_builder.Finish(&cbg_array);
    std::shared_ptr<arrow::Array> date_array;
    status = _date_builder.Finish(&date_array);
    std::shared_ptr<arrow::Array> rank_array;
    status = _rank_builder.Finish(&rank_array);

    auto schema = arrow::schema(
            {arrow::field("cbg", arrow::utf8()),
             arrow::field("date", arrow::date32()),
             arrow::field("rank", arrow::float64())
            });

    auto table = arrow::Table::Make(schema, {cbg_array, date_array, rank_array});
    status = p.write(*table);
    spdlog::info("All done");

    return hpx::finalize();
}


int main(int argc, char **argv) {
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
            ("nr", value<uint16_t>()->default_value(60), "Number of simultaneous rows to process")
            ("np", value<uint16_t>()->default_value(1),
             "Number of partitions for each file (CBGs to process)")
            ("silent", "disable debug logging")
            ("config", value<string>()->default_value("./config.yml"), "Config file location");

    return hpx::init(desc_commandline, argc, argv);
}