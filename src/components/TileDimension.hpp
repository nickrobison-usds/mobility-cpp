//
// Created by Nicholas Robison on 6/1/20.
//

#ifndef MOBILITY_CPP_TILEDIMENSION_HPP
#define MOBILITY_CPP_TILEDIMENSION_HPP

#include "components/serializers.hpp"

#include <cstddef>

using namespace std;

namespace components {
    struct TileDimension {
        std::size_t _cbg_offset; // Min X/Y
        std::size_t _cbg_count;  // Max X/Y
        std::size_t _time_offset; // Min Z
        std::size_t _time_count; // Max Z
        std::string _cbg_shp;
        std::string _poi_parquet;

        friend class hpx::serialization::access;

        template<typename Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & _cbg_offset & _cbg_count & _time_offset & _time_count & _cbg_shp & _poi_parquet;
        }
    };
}


#endif //MOBILITY_CPP_TILEDIMENSION_HPP
