//
// Created by Nicholas Robison on 6/1/20.
//

#ifndef MOBILITY_CPP_TILECONFIGURATION_HPP
#define MOBILITY_CPP_TILECONFIGURATION_HPP

#include "components/serializers.hpp"

#include <cstddef>

using namespace std;

namespace components {
    struct TileConfiguration {
        std::string _filename;
        std::size_t _cbg_min; // Min X
        std::size_t _cbg_max;  // Max X
        std::size_t _time_offset; // Min Z
        std::size_t _time_count; // Max Z
        std::string _cbg_shp;
        std::string _poi_parquet;
        std::uint16_t _nr; // Number of concurrent rows

        friend class hpx::serialization::access;

        template<typename Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & _filename & _cbg_min & _cbg_max & _time_offset & _time_count & _cbg_shp & _poi_parquet & _nr;
        }
    };
}


#endif //MOBILITY_CPP_TILECONFIGURATION_HPP
