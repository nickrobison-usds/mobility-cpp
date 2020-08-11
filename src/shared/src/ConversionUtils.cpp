//
// Created by Nicholas Robison on 7/30/20.
//

#include <spdlog/spdlog.h>
#include "shared/ConversionUtils.hpp"

namespace shared {


    template <>
    std::uint16_t ConversionUtils::convert_empty(const std::string &val) {
        std::uint16_t v = 0;
        try {
            v = std::stoi(val);
        } catch (std::invalid_argument &e) {
            spdlog::error("Cannot convert {}", val);
        }
        return v;
    }

    template <>
    std::uint32_t ConversionUtils::convert_empty(const std::string &val) {
        std::uint32_t v = 0;
        try {
            v = std::stol(val);
        } catch (std::invalid_argument &e) {
            spdlog::error("Cannot convert {}", val);
        }
        return v;
    }

    template <>
    std::uint64_t ConversionUtils::convert_empty(const std::string &val) {
        std::uint64_t v = 0;
        try {
            v = std::stol(val);
        } catch (std::invalid_argument &e) {
            spdlog::error("Cannot convert {}", val);
        }
        return v;
    }
}