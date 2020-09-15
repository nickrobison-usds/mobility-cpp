//
// Created by Nicholas Robison on 6/1/20.
//

#include <shared/constants.hpp>
#include <components/TileClient.hpp>
#include <shared/constants.hpp>
#include <shared/HostnameLogger.hpp>
#include <shared/DateUtils.hpp>
#include <shared/DirectoryUtils.hpp>
#include <hpx/program_options.hpp>
#include <hpx/hpx_init.hpp>
#include "spdlog/spdlog.h"
#include "spdlog/pattern_formatter.h"
#include <chrono>
#include <string>
#include <hpx/runtime/find_localities.hpp>

using namespace std;

typedef chrono::duration<int, ratio_multiply<chrono::hours::period, ratio<24>>::type> days;


int hpx_main(hpx::program_options::variables_map &vm) {
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<shared::HostnameLogger>('h').set_pattern("[%l] [%h] [%H:%M:%S %z] [thread %t] %v");
    spdlog::set_formatter(std::move(formatter));
    spdlog::info("Initializing connectivity calculator on locale {}", hpx::get_locality_id());

    if (!vm.count("silent")) {
        spdlog::set_level(spdlog::level::debug);
    }

    // Build the file paths
    const string input_dir = vm["data_dir"].as<string>();
    const fs::path data_dir(fs::absolute(fs::path(input_dir)));
    const auto cbg_str = vm["cbg_shp"].as<string>();
    const auto cbg_path = shared::DirectoryUtils::build_path(data_dir, cbg_str);
    const auto poi_str = vm["poi_parquet"].as<string>();
    const auto poi_path = shared::DirectoryUtils::build_path(data_dir, poi_str);
    const auto csv_str = vm["pattern_csvs"].as<string>();
    const auto csv_path = shared::DirectoryUtils::build_path(data_dir, csv_str);
    const auto output_str = vm["output_directory"].as<string>();
    const auto output_path = shared::DirectoryUtils::build_path(data_dir, output_str);
    const auto output_name = vm["output_name"].as<string>();
    const auto nr = vm["nr"].as<uint16_t>();
    const auto tile_parititions = vm["np"].as<uint16_t>();

    if (!fs::exists(output_path)) {
        spdlog::debug("Creating output directory");
        fs::create_directory(output_path);
    }

    const string date_string = vm["start_date"].as<string>();
    // TODO: Would be nice to combine this with the previous call.
    const auto start_date = shared::DateUtils::parse_date(date_string);

    // Set the end date, if one is provided
    const string end_date_string = vm["end_date"].as<string>();
    chrono::system_clock::time_point end_date;
    if (end_date_string.empty()) {
        end_date = chrono::system_clock::now();
    } else {
        end_date = shared::DateUtils::parse_date(end_date_string);
    }
    // Max size of Z axis
    const auto time_bounds = chrono::duration_cast<days>(end_date - start_date).count();

    const auto locales = hpx::find_all_localities();
    spdlog::debug("Executing on {} locales", locales.size());

    const auto files = shared::DirectoryUtils::enumerate_files(csv_path.string(), ".*patterns\\.csv");
    // From the list of files, get their paths (as strings) and compute their offset from the
    vector<pair<string, date::sys_days>> input_files;
    std::transform(files.begin(), files.end(), back_inserter(input_files), [&start_date](const auto &file) {
        const fs::path p = file.path();
        const auto f = p.filename();
        // Try to parse out the date
        std::istringstream fstream{p.filename().string()};
        date::sys_days file_date;
        fstream >> date::parse("%F", file_date);
        string fp = file.path().string();
        return make_pair(fp,
                         file_date);
    });
    // Filter out everything that's not within our date range
    input_files.erase(
            std::remove_if(input_files.begin(), input_files.end(), [&start_date, &end_date](const auto &pair) {
                auto before_dif = chrono::duration_cast<days>(pair.second - start_date).count();
                auto after_dif = chrono::duration_cast<days>(pair.second - end_date).count();
                return !(before_dif >= 0 && after_dif <= 0);
            }), input_files.end());

    // For each file, we need to build a tile parameter, partitioning if necessary
    vector<components::TileConfiguration> tiles;
    std::for_each(input_files.begin(), input_files.end(),
                  [&tiles, &tile_parititions, &cbg_path, &poi_path, &nr, &start_date](const auto &pair) {
                      const size_t ds = chrono::duration_cast<days>(start_date.time_since_epoch()).count();
                      const auto stride = shared::MAX_CBG / tile_parititions;

                      for (size_t i = 0; i < shared::MAX_CBG; i += stride) {
                          components::TileConfiguration dim{pair.first, i, std::min(i + stride, shared::MAX_CBG),
                                                            ds, 7, nr};
                          tiles.push_back(dim);
                      }
                  });

    // Partition the inputs based on the number of locales;
    const auto split_tiles = shared::DirectoryUtils::split_vector(tiles, locales.size());

    const auto locale_tiles = split_tiles.at(hpx::get_locality_id());

    components::TileClient t(output_path.string(), output_name, cbg_path.string(), poi_path.string());

    for (const auto &tile : locale_tiles) {
        auto init_future = t.init(tile, 1);
        init_future.get();
//        }
    };

    return hpx::finalize();
}

int main(int argc, char **argv) {
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
            ("start_date", value<string>()->default_value("2020-03-01"), "First date to handle")
            ("end_date", value<string>()->default_value(""), "Last date to handle")
            ("data_dir", value<string>()->default_value("test-dir/"), "Root of data directory")
            ("cbg_shp", value<string>()->default_value("reference/census/block_groups.shp"), "CBG shapefile")
            ("poi_parquet", value<string>()->default_value("reference/Joined_POI.parquet"),
             "Parquet file with POI information")
            ("pattern_csvs", value<string>()->default_value("safegraph/weekly-patterns/"),
             "Directory with weekly pattern files")
            ("output_directory", value<string>()->default_value("output/"), "Where to place the output files")
            ("output_name", value<string>()->default_value("mobility_matrix"), "Name of output files")
            ("nr", value<uint16_t>()->default_value(60), "Number of simultaneous rows to process")
            ("np", value<uint16_t>()->default_value(1),
             "Number of partitions for each file (CBGs to process)")
            ("silent", "disable debug logging");

    std::vector<std::string> const cfg = {
            "hpx.run_hpx_main!=1"
    };

    return hpx::init(desc_commandline, argc, argv, cfg);
}