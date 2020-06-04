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

using namespace std;

typedef chrono::duration<int, ratio_multiply<chrono::hours::period, ratio<24>>::type> days;

chrono::system_clock::time_point parse_date(const string &date) {
    tm tm = {};
    stringstream ss{date};
    ss >> get_time(&tm, "%Y-%m-%d");
    chrono::system_clock::time_point tp = chrono::system_clock::from_time_t(mktime(&tm));

    return tp;
}

int hpx_main(hpx::program_options::variables_map &vm) {
    spdlog::set_level(spdlog::level::debug);
    spdlog::set_pattern("[%H:%M:%S %z] [thread %t] %v");
    spdlog::info("Initializing connectivity calculator");

    const string input_dir = vm["input_dir"].as<string>();
    const string date_string = vm["start_date"].as<string>();
    const size_t min_cbg = vm["min_cbg"].as<size_t>();
    const size_t max_cbg = vm["min_cbg"].as<size_t>();
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
    // Max size of X, Y axis
    const size_t cbg_bounds = max_cbg - min_cbg;

    const auto locales = hpx::find_all_localities();
    spdlog::debug("Executing on {} locales", locales.size());

    const auto files = partition_files(input_dir, locales.size(), ".*patterns\\.csv");
    vector <string> f;
    std::transform(files[0].begin(), files[0].end(), back_inserter(f), [](const auto &file) {
        return file.path().string();
    });

    // Create the Tile Server and start it up
    components::TileDimension dim;
    dim._cbg_count = 100;
    dim._time_count = 10;
    components::TileClient t(hpx::find_here());
    auto init_future = t.init(f[0], dim, 1);
    init_future.get();


    return hpx::finalize();
}

int main(int argc, char **argv) {
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
            ("start_date", value<string>()->default_value("2020-03-01"), "First date to handle")
            ("end_date", value<string>()->default_value(""), "Last date to handle")
            ("min_cbg", value<size_t>()->default_value(1), "Minimum CBG ID")
            ("max_cbg", value<size_t>()->default_value(10), "Maximum CBG ID")
            ("input_dir", value<string>()->default_value("./test-dir"), "Input directory to parse");

    return hpx::init(desc_commandline, argc, argv);
}