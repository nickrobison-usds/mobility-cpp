//
// Created by Nicholas Robison on 7/27/20.
//

#include <shared/HostnameLogger.hpp>
#include <hpx/program_options.hpp>
#include <hpx/hpx_init.hpp>
#include <yaml-cpp/yaml.h>
#include "spdlog/spdlog.h"
#include "spdlog/pattern_formatter.h"

#include "config.cpp"

using namespace std;

int hpx_main(hpx::program_options::variables_map &vm) {
    auto formatter = std::make_unique<spdlog::pattern_formatter>();
    formatter->add_flag<shared::HostnameLogger>('h').set_pattern("[%l] [%h] [%H:%M:%S %z] [thread %t] %v");
    spdlog::set_formatter(std::move(formatter));
    spdlog::info("Initializing centrality calculator on locale {}", hpx::get_locality_id());

    if (!vm.count("silent")) {
        spdlog::set_level(spdlog::level::debug);
    }

    const auto config_path = vm["config_file"].as<string>();

    const auto config = YAML::LoadFile(config_path).as<CentralityConfig>();

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
            ("silent", "disable debug logging")
            ("config_file", value<string>()->default_value("./config.yml"), "Config file location");

    return hpx::init(desc_commandline, argc, argv);
}