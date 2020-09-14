//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_MAPTILEBUILDER_HPP
#define MOBILITY_CPP_MAPTILEBUILDER_HPP

#include "MapTile.hpp"
#include <array>
#include <string_view>
#include <utility>

namespace fs = boost::filesystem;

namespace mt {
    template<
            class MapKey,
            class Coordinate,
            class Mapper,
            class Tiler,
            class ReduceValue,
            class InputKey = std::string,
            template<typename = InputKey> class Provider = io::FileProvider
    >
    class MapTileBuilder {
    public:
        explicit MapTileBuilder(const Coordinate &min, const Coordinate &max, fs::path directory): _min(min), _max(max), _directory(std::move(directory)), _regex("*") {
            // Not used
        }

        MapTileBuilder &set_stride(std::array<std::size_t, Coordinate::dimensions> &stride) {
            _stride = stride;
            return *this;
        }

        MapTileBuilder &add_config_value(const std::string_view key, const std::string_view value) {
            _config_values[key] = value;
            return *this;
        }

        MapTileBuilder &set_config_values(const std::map<std::string, std::string> &values) {
            _config_values = values;
            return *this;
        }

        MapTileBuilder &set_regex(const std::string_view regex) {
            _regex = regex;
            return *this;
        }

        MapTile<MapKey, Coordinate, Mapper, Tiler, ReduceValue> build() {
            return MapTile<MapKey, Coordinate, Mapper, Tiler, ReduceValue>(_min, _max, _stride, _directory, _regex, _config_values);
        }

    private:
        std::array<std::size_t, Coordinate::dimensions> _stride;
        const Coordinate _min;
        const Coordinate _max;
        const fs::path _directory;
        std::string _regex;
        std::map<std::string, std::string> _config_values;
    };
}


#endif //MOBILITY_CPP_MAPTILEBUILDER_HPP
