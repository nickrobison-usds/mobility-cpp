//
// Created by Nicholas Robison on 6/1/20.
//

#ifndef MOBILITY_CPP_TILECONFIGURATION_HPP
#define MOBILITY_CPP_TILECONFIGURATION_HPP

#include "shared/serializers.hpp"

#include <cstddef>

using namespace std;

namespace components {
    struct TileConfiguration {
        std::string _filename;
        std::size_t _tile_min; // Min X
        std::size_t _tile_max;  // Max X
        std::size_t _time_offset; // Min Z
        std::size_t _time_count; // Max Z
        std::uint16_t _nr; // Number of concurrent rows

        friend class hpx::serialization::access;

        template<typename Archive>
        void serialize(Archive &ar, const unsigned int version) {
            ar & _filename & _tile_min & _tile_max & _time_offset & _time_count & _nr;
        }
    };
}


#endif //MOBILITY_CPP_TILECONFIGURATION_HPP
