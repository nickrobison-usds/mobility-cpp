//
// Created by Nicholas Robison on 7/27/20.
//


#include "config.cpp"
#include "SafegraphMapper.hpp"
#include "SafegraphTiler.hpp"
#include <hpx/program_options.hpp>
#include <hpx/hpx_init.hpp>
#include <map-tile/client/MapTileClient.hpp>
#include <map-tile/coordinates/LocaleTiler.hpp>
#include <map-tile/coordinates/LocaleLocator.hpp>
#include <map-tile/coordinates/Coordinate3D.hpp>
#include <map-tile/io/FileProvider.hpp>
#include <shared/HostnameLogger.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <shared/DateUtils.hpp>
#include <shared/DirectoryUtils.hpp>
#include <shared/data.hpp>

#include "spdlog/spdlog.h"
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

    if (!vm.count("silent")) {
        spdlog::set_level(spdlog::level::debug);
    }

    const auto config_path = vm["config"].as<string>();
    const auto config = YAML::LoadFile(config_path).as<CentralityConfig>();

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
    spdlog::debug("Executing on {} locales", locales.size());
    using namespace mt::coordinates;
    // Build the dataset space
    // Compute the Z-index, the number of days in the analysis
    const auto time_bounds = chrono::duration_cast<shared::days>(config.end_date - config.start_date).count();
    const std::array<std::size_t, 3> stride{static_cast<std::size_t>(time_bounds),
                                            static_cast<std::size_t>(floor(MAX_CBG / locales.size())), MAX_CBG};
    const auto tiles = LocaleTiler::tile<Coordinate3D>(Coordinate3D(0, 0, 0),
                                                       Coordinate3D(time_bounds, MAX_CBG, MAX_CBG), stride);
    spdlog::debug("Partitioned into {} tiles", tiles.size());
    const LocaleLocator<Coordinate3D> locator(tiles);

    const auto sd = chrono::floor<date::days>(config.start_date);
    const auto ed = chrono::floor<date::days>(config.end_date);

    std::map<std::string, std::string> config_values;
    config_values["poi_path"] = poi_path.string();
    config_values["cbg_path"] = cbg_path.string();
    config_values["start_date"] = std::to_string(sd.time_since_epoch().count());
    config_values["end_date"] = std::to_string(ed.time_since_epoch().count());
    config_values["output_dir"] = config.output_dir;
    config_values["output_name"] = config.output_name;

    if (locales.size() != tiles.size()) {
        spdlog::warn("Cannot executing {} tiles in {} locales.", tiles.size(), locales.size());
    }

    // Initialize all the locales
    vector<mt::client::MapTileClient<v2, Coordinate3D, SafegraphMapper, SafegraphTiler>> servers;


    // Partition the input files, one for each tile
    // Get the input files
    const auto csv_path = shared::DirectoryUtils::build_path(config.data_dir, config.patterns_csv);
    const auto files = shared::DirectoryUtils::partition_files(
            csv_path.string(),
            tiles.size(),
            ".*patterns\\.csv");

    // Create a a server for each tile, cycling through the locales and files
    const auto locale_range = ranges::views::cycle(locales);
    const auto z = ranges::views::zip(locale_range, tiles, files);

    ranges::for_each(
            z,
            [&servers, &locator, &config_values](
                    const auto &pair) {
                const auto& files = get<2>(pair);
                vector<string> file_strs;
                transform(files.begin(), files.end(), back_inserter(file_strs), [](const auto &f) {
                    return f.path().string();
                });
                const auto tile = get<1>(pair).first;
                spdlog::debug("Creating server on locale {}", get<0>(pair));
//                const auto s = fmt::format("Tile bounds: {} - {}", tile.min_corner(), tile.max_corner());
                mt::client::MapTileClient<v2, Coordinate3D, SafegraphMapper, SafegraphTiler> server(get<0>(pair), locator,
                                                                                                    tile,
                                                                                                    config_values,
                                                                                                    file_strs);
                servers.push_back(std::move(server));
            });

    vector<hpx::future<void>> results;
    std::transform(servers.begin(), servers.end(), std::back_inserter(results), [](auto &server) {
        return std::move(server.tile());
    });

    hpx::wait_all(results);
    spdlog::info("Tile complete, beginning computation");

    // Now, compute
    vector<hpx::future<void>> compute_results;
    std::transform(servers.begin(), servers.end(), std::back_inserter(compute_results), [](auto &mt) {
        return std::move(mt.compute());
    });

    hpx::wait_all(compute_results);

    spdlog::debug("Computing completed");

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