//
// Created by Nicholas Robison on 7/27/20.
//


#include "config.cpp"
#include "SafegraphMapper.hpp"
#include "SafegraphTiler.hpp"
#include <hpx/program_options.hpp>
#include <hpx/hpx_init.hpp>
#include <hpx/modules/segmented_algorithms.hpp>
#include <map-tile/client/MapTileClient.hpp>
#include <map-tile/coordinates/LocaleTiler.hpp>
#include <map-tile/coordinates/LocaleLocator.hpp>
#include <map-tile/coordinates/Coordinate3D.hpp>
#include <map-tile/io/FileProvider.hpp>
#include <shared/HostnameLogger.hpp>
#include <shared/DateUtils.hpp>
#include <shared/DirectoryUtils.hpp>
#include <shared/data.hpp>

#include "spdlog/spdlog.h"
#include <spdlog/fmt/fmt.h>
#include "spdlog/pattern_formatter.h"

// The total number of Census Block Groups (CBGs) in the US
const static std::size_t MAX_CBG = 220740;

using namespace std;

// Register the map-tile instance
REGISTER_MAPPER(v2, mt::coordinates::Coordinate3D, SafegraphMapper, SafegraphTiler, string, mt::io::FileProvider);

int hpx_main(hpx::program_options::variables_map &vm) {
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<shared::HostnameLogger>('h').set_pattern("[%l] [%h] [%H:%M:%S %z] [thread %t] %v");
    spdlog::set_formatter(std::move(formatter));
    spdlog::info("Initializing centrality calculator on locale {}", hpx::get_locality_id());

    if (!vm.count("silent")) {
        spdlog::set_level(spdlog::level::debug);
    }

    const auto config_path = vm["config"].as<string>();
    const auto config = YAML::LoadFile(config_path).as<CentralityConfig>();

    // Compute the Z-index, the number of days in the analysis
    const auto time_bounds = chrono::duration_cast<shared::days>(config.end_date - config.start_date).count();
    // Build the dataset space
    const std::array<std::size_t, 3> stride{7, MAX_CBG, MAX_CBG};

    // Get the input files
    const auto csv_path = shared::DirectoryUtils::build_path(config.data_dir, config.patterns_csv);
    const auto files = shared::DirectoryUtils::enumerate_files(
            csv_path.string(),
            ".*patterns\\.csv");

    vector<string> file_strs;
    transform(files.begin(), files.end(), back_inserter(file_strs), [](const auto &f) {
        return f.path().string();
    });

    // Build the reference file paths
    const auto cbg_path = shared::DirectoryUtils::build_path(config.data_dir, config.cbg_shp);
    const auto poi_path = shared::DirectoryUtils::build_path(config.data_dir, config.poi_parquet);
    const auto output_path = shared::DirectoryUtils::build_path(config.data_dir, config.output_dir);

    if (!fs::exists(output_path)) {
        spdlog::debug("Creating output directory");
        fs::create_directory(output_path);
    }

    // Tile the input space
    const auto locales = hpx::find_all_localities();
    using namespace mt::coordinates;
    const auto tiles = LocaleTiler::tile<Coordinate3D>(Coordinate3D(0, 0, 0),
                                                       Coordinate3D(time_bounds, MAX_CBG, MAX_CBG), stride);
    const LocaleLocator<Coordinate3D> locator(tiles);

    const auto sd = chrono::floor<date::days>(config.start_date);
    const auto ed = chrono::floor<date::days>(config.end_date);


    std::map<std::string, std::string> config_values;
    config_values["poi_path"] = poi_path.string();
    config_values["cbg_path"] = cbg_path.string();
    config_values["start_date"] = std::to_string(sd.time_since_epoch().count());
    config_values["end_date"] = std::to_string(ed.time_since_epoch().count());

    using hpx::util::make_zip_iterator;
    using hpx::util::tuple;
    using hpx::util::get;



    vector<hpx::future<void>> results;

    std::for_each(
            make_zip_iterator(locales.begin(), tiles.begin()),
            make_zip_iterator(locales.end(), tiles.end()),
            [&results, &locator, &file_strs, &config_values](tuple<const hpx::id_type, const LocaleLocator<Coordinate3D>::value> t) {
                mt::client::MapTileClient<v2, Coordinate3D, SafegraphMapper, SafegraphTiler> server(get<0>(t), locator,
                                                                                                    get<1>(t).first,
                                                                                                    config_values,
                                                                                                    file_strs);
                results.push_back(std::move(server.tile()));
            });

    hpx::wait_all(results);

    return hpx::finalize();
}


int main(int argc, char **argv) {
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
            ("output_name", value<string>()->default_value("mobility_matrix"), "Name of output files")
            ("nr", value<uint16_t>()->default_value(60), "Number of simultaneous rows to process")
            ("np", value<uint16_t>()->default_value(1),
             "Number of partitions for each file (CBGs to process)")
            ("silent", "disable debug logging")
            ("config", value<string>()->default_value("./config.yml"), "Config file location");

    return hpx::init(desc_commandline, argc, argv);
}