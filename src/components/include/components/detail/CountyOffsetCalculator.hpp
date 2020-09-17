//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_COUNTYOFFSETCALCULATOR_HPP
#define MOBILITY_CPP_COUNTYOFFSETCALCULATOR_HPP

#include "components/OffsetCalculator.hpp"
#include <shared/TileConfiguration.hpp>
#include "offset_shared.hpp"

namespace components::detail {
    class CountyOffsetCalculator : public OffsetCalculator<CountyOffsetCalculator> {
    public:
        typedef std::vector<std::pair<std::string, std::size_t>> offset_type;

        explicit CountyOffsetCalculator(const offset_type &init, const TileConfiguration &config);

        [[nodiscard]] std::optional<std::size_t> to_global_offset_impl(const std::string_view fips_code) const;

        [[nodiscard]] std::size_t to_local_offset_impl(const std::string_view fips_code) const;

        [[nodiscard]] std::optional<std::string> from_local_offset_impl(size_t fips_idx) const;

        [[nodiscard]] optional<string> from_global_offset_impl(size_t fips_idx) const;

    private:
        const std::size_t _start_idx;
        const std::size_t _end_idx;
        const offset_bimap _fips_map;
    };
}


#endif //MOBILITY_CPP_COUNTYOFFSETCALCULATOR_HPP
