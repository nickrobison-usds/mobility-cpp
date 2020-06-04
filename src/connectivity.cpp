//
// Created by Nicholas Robison on 6/1/20.
//

#include <components/TileClient.hpp>
#include <hpx/program_options.hpp>
#include <hpx/hpx_init.hpp>
#include "spdlog/spdlog.h"
#include "utils.hpp"
#include <iostream>
#include <chrono>
#include <sstream>
#include <string>
#include <iomanip>
#include <hpx/runtime/find_localities.hpp>

// The smallest and largest CBG codes. These shouldn't ever change
const static uint64_t min_cbg = 010010201001;
const static uint64_t max_cbg = 780309900000;
// Max size of X, Y axis
const static uint64_t cbg_bounds = max_cbg - min_cbg;

using namespace std;

typedef chrono::duration<int, ratio_multiply<chrono::hours::period, ratio<24>>::type> days;

chrono::system_clock::time_point parse_date(const string &date) {
    tm tm = {};
    stringstream ss{date};
    ss >> get_time(&tm, "%Y-%m-%d");
    chrono::system_clock::time_point tp = chrono::system_clock::from_time_t(mktime(&tm));

    return tp;
}

fs::path buildPath(const fs::path &root_path, const string &path_string) {
    fs::path appender(path_string);
    if (appender.is_relative()) {
        return fs::path(root_path) /= appender;
    };
    return appender;
};


int hpx_main(hpx::program_options::variables_map &vm) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%H:%M:%S %z] [thread %t] %v");
    spdlog::info("Initializing connectivity calculator");

    // Build the file paths
    const string input_dir = vm["data_dir"].as<string>();
    const fs::path data_dir(fs::absolute(fs::path(input_dir)));
    const auto cbg_str = vm["cbg_shp"].as<string>();
    const auto cbg_path = buildPath(data_dir, cbg_str);
    const auto poi_str = vm["poi_parquet"].as<string>();
    const auto poi_path = buildPath(data_dir, poi_str);
    const auto csv_str = vm["pattern_csvs"].as<string>();
    const auto csv_path = buildPath(data_dir, csv_str);


    const string date_string = vm["start_date"].as<string>();
    // TODO: Would be nice to combine this with the previous call.
    const auto start_date = parse_date(date_string);

    // Set the end date, if one is provided
    const string end_date_string = vm["end_date"].as<string>();
    chrono::system_clock::time_point end_date;
    if (end_date_string.empty()) {
        end_date = chrono::system_clock::now();
    } else {
        end_date = parse_date(end_date_string);
    }
    // Max size of Z axis
    const auto time_bounds = chrono::duration_cast<days>(end_date - start_date).count();

    const auto locales = hpx::find_all_localities();
    spdlog::debug("Executing on {} locales", locales.size());

    const auto files = partition_files(csv_path.string(), locales.size(), ".*patterns\\.csv");
    vector<string> f;
    std::transform(files[0].begin(), files[0].end(), back_inserter(f), [](const auto &file) {
        return file.path().string();
    });

    // Create the Tile Server and start it up
    components::TileDimension dim{0, 100, 10, 0, cbg_path.string(), poi_path.string()};
    components::TileClient t(dim);
    auto init_future = t.init(f[0], 1);
    init_future.get();


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
             "Directory with weekly pattern files");

    return hpx::init(desc_commandline, argc, argv);
}