//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_OFFSETCALCULATOR_HPP
#define MOBILITY_CPP_OFFSETCALCULATOR_HPP

#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace components {
    template<typename Calculator>
    class OffsetCalculator {
    public:
        std::optional<std::size_t> to_global_offset(const std::string_view value) const {
            return static_cast<Calculator const &> (*this).to_global_offset_impl(value);
        }

        std::size_t to_local_offset(const std::string_view value) const {
            return static_cast<Calculator const &> (*this).to_local_offset_impl(value);
        }

        std::optional<std::string> from_global_offset(const std::size_t offset) const {
            return static_cast<Calculator const &> (*this).from_global_offset_impl(offset);
        }

        std::optional<std::string> from_local_offset(const std::size_t offset) const {
            return static_cast<Calculator const &> (*this).from_local_offset_impl(offset);
        }
    };
}


#endif //MOBILITY_CPP_OFFSETCALCULATOR_HPP
