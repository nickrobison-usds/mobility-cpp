//
// Created by Nicholas Robison on 9/11/20.
//

#include "EpiConfig.cpp"
#include "SafegraphCountyMapper.hpp"
#include "SafegraphCountyTiler.hpp"
#include <hpx/program_options.hpp>
#include <hpx/hpx_init.hpp>
#include <map-tile/MapTileBuilder.hpp>
#include <map-tile/client/MapTileClient.hpp>
#include <map-tile/coordinates/Coordinate3D.hpp>
#include <map-tile/io/FileProvider.hpp>
#include <shared/constants.hpp>
#include <shared/data.hpp>

// Register the map-tile instance
typedef vector<cbg_centrality> reduce_type;
REGISTER_MAPPER(county_visit, mt::coordinates::Coordinate3D, SafegraphCountyMapper, SafegraphCountyTiler, reduce_type, string,
        mt::io::FileProvider);

int hpx_main(hpx::program_options::variables_map &vm) {

    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<shared::HostnameLogger>('h').set_pattern("[%l] [%h] [%H:%M:%S %z] [thread %t] %v");
    spdlog::set_formatter(std::move(formatter));

    if (!vm.count("silent")) {
        spdlog::set_level(spdlog::level::debug);
    }

    const auto config_path = vm["config"].as<string>();
    const auto config = YAML::LoadFile(config_path).as<EpiConfig>();

    // Build the reference file paths
    const auto cbg_path = shared::DirectoryUtils::build_path(config.data_dir, config.cbg_shp);
    const auto county_path = shared::DirectoryUtils::build_path(config.data_dir, config.county_shp);
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
    // We'll stride by date, that way we have a complete matrix on each locale.
    std::array<std::size_t, 3> stride{static_cast<std::size_t>(floor(time_bounds / locales.size())),
                                      shared::MAX_COUNTY, shared::MAX_COUNTY};

    spdlog::debug("Stride: {}/{}/{}", stride[0], stride[1], stride[2]);

    const auto sd = chrono::floor<date::days>(config.start_date);
    const auto ed = chrono::floor<date::days>(config.end_date);

    std::map<std::string, std::string> config_values;
    config_values["poi_path"] = poi_path.string();
    config_values["cbg_path"] = cbg_path.string();
    config_values["county_path"] = county_path.string();
    config_values["start_date"] = std::to_string(sd.time_since_epoch().count());
    config_values["end_date"] = std::to_string(ed.time_since_epoch().count());
    config_values["output_dir"] = output_path.string();
    config_values["output_name"] = config.output_name;
    config_values["log_dir"] = log_path.string();

    // Partition the input files, try one for each tile
    const auto csv_path = shared::DirectoryUtils::build_path(config.data_dir, config.patterns_csv);


// Create the client using the builder
    mt::MapTileBuilder<county_visit, Coordinate3D, SafegraphCountyMapper, SafegraphCountyTiler, reduce_type> builder(Coordinate3D(0, 0, 0),
                                                                                                     Coordinate3D(
                                                                                                             time_bounds,
                                                                                                             shared::MAX_COUNTY,
                                                                                                             shared::MAX_COUNTY),
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
    spdlog::info("All compution completed");

    return hpx::finalize();
};

int main(int argc, char **argv) {
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
            ("config", value<std::string>()->default_value("./config.yml"), "Config file location")
            ("silent", "disable debug logging");

    return hpx::init(desc_commandline, argc, argv);
}
