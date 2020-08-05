//
// Created by Nicholas Robison on 7/27/20.
//

#include <yaml-cpp/yaml.h>
#include <shared/DateUtils.hpp>
#include <string>

struct CentralityConfig {

    std::chrono::system_clock::time_point start_date;
    std::chrono::system_clock::time_point end_date;
    std::string data_dir;
    std::string cbg_shp;
    std::string poi_parquet;
    std::string patterns_csv;
    std::string output_dir;
    std::string output_name;
};

namespace YAML {

    template<>
    struct convert<CentralityConfig> {

        static Node encode(const CentralityConfig &rhs) {
            Node node;
            return node;
        }

        static bool decode(const Node &node, CentralityConfig &rhs) {
            if(!node.IsMap()) {
                return false;
            }

            rhs.start_date = node["start_date"].as<std::chrono::system_clock::time_point>();
            rhs.end_date = node["end_date"].as<std::chrono::system_clock::time_point>();
            rhs.data_dir = node["data_dir"].as<std::string>();
            rhs.cbg_shp = node["cbg_shp"].as<std::string>();
            rhs.poi_parquet = node["poi_parquet"].as<std::string>();
            rhs.patterns_csv = node["patterns_csv"].as<std::string>();
            rhs.output_dir = node["output_dir"].as<std::string>();
            rhs.output_name = node["output_name"].as<std::string>();
            return true;
        }
    };

    template<>
    struct convert<std::chrono::system_clock::time_point> {
        typedef std::chrono::system_clock::time_point tp;

        static Node encode(const tp &rhs) {
            Node node;

            return node;
        }

        static bool decode(const Node &node, tp &rhs) {
            if(!node.IsScalar()) {
                return false;
            }

            node.IsScalar();
            const auto data_str = node.as<std::string>();
            rhs = shared::DateUtils::parse_date(data_str);
            return true;
        }
    };
}
