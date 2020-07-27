//
// Created by Nicholas Robison on 7/27/20.
//

#include <hpx/program_options.hpp>
#include <hpx/hpx_init.hpp>
#include <shared/HostnameLogger.hpp>
#include <shared/DateUtils.hpp>
#include <shared/DirectoryUtils.hpp>
#include <yaml-cpp/yaml.h>
#include "spdlog/spdlog.h"
#include "spdlog/pattern_formatter.h"

#include "config.cpp"

// The total number of Census Block Groups (CBGs) in the US
const static std::size_t MAX_CBG = 220740;

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

    // Compute the Z-index, the number of days in the analysis
    const auto time_bounds = chrono::duration_cast<shared::days>(config.end_date - config.start_date).count();
    // Build the dataset space
    const std::array<std::size_t, 3> dimensions{7, MAX_CBG, MAX_CBG};

    // Get the input files
    const auto files = shared::DirectoryUtils::enumerate_files(shared::DirectoryUtils::build_path(config.data_dir, "safegraph/weekly-patterns/").string(), ".*patterns\\.csv");

    // Tile the input space


    return hpx::finalize();
}


int main(int argc, char **argv) {
    using namespace hpx::program_options;

    options_description desc_commandline;
    desc_commandline.add_options()
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