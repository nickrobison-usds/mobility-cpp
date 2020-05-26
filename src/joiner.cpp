//
// Created by Nicholas Robison on 5/24/20.
//

#include <algorithm>
#include "components/data.hpp"
#include "components/LocationJoiner.hpp"
#include <hpx/hpx_init.hpp>
#include <hpx/hpx.hpp>
#include "utils.hpp"
#include "spdlog/spdlog.h"
#include "spdlog/fmt/fmt.h" // Get FMT from spdlog, to avoid conflicts with other libraries.
#include "LocationJoinWriter.hpp"

using namespace std;

int hpx_main(hpx::program_options::variables_map &vm) {
    string input_dir = vm["input_dir"].as<string>();
    string output_file = vm["output_file"].as<string>();
    string shapefile = vm["shapefile"].as<string>();

    spdlog::set_level(spdlog::level::debug);
    spdlog::info("Initializing Location Joiner");

    const auto locales = hpx::find_all_localities();
    spdlog::debug("Executing on {} locales", locales.size());

    vector<hpx::future<vector<safegraph_location>>> results;
    results.reserve(locales.size());

    const auto files = partition_files(input_dir, locales.size(), ".*\\.csv");
    vector<string> f;
    std::transform(files[0].begin(), files[0].end(), back_inserter(f), [](const auto &file) {
        return file.path().string();
    });

    for (const auto &node : locales) {
        const auto l = hpx::new_<components::LocationJoiner>(node, f, shapefile);
        results.push_back(l.invoke());
    }

    const LocationJoinWriter writer(output_file);

    const auto r2 = hpx::when_each([&writer](hpx::future<vector<safegraph_location>> f) {
        const auto v = f.get();
        spdlog::debug("I have {} elements", v.size());
        writer.write(v);
    }, results);

    return hpx::finalize();
}

int main(int argc, char **argv) {
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()("input_dir", value<string>()->default_value("data"),
                                   "Input directory to parse")("output_file",
                                                               value<string>()->default_value("./wrong.parquet"),
                                                               "output file to write")("shapefile", value<string>(),"Input Census shapefile");

    return hpx::init(desc_commandline, argc, argv);
}