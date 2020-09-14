//
// Created by Nicholas Robison on 9/14/20.
//

#include "components/detail/CountyOffsetCalculator.hpp"
#include <spdlog/spdlog.h>

namespace components::detail {

    offset_bimap build_offset_map(const CountyOffsetCalculator::offset_type &offsets) {
        detail::offset_bimap cbg_offsets;

        std::for_each(offsets.begin(), offsets.end(), [&cbg_offsets](const auto &pair) {
            cbg_offsets.insert(detail::position(pair.first, pair.second));
        });

        return cbg_offsets;
    };

    CountyOffsetCalculator::CountyOffsetCalculator(const CountyOffsetCalculator::offset_type &init,
                                                   const TileConfiguration &config) : _fips_map(build_offset_map(init)),
                                                                                      _start_idx(config._tile_min),
                                                                                      _end_idx(config._tile_max) {
        // Not used
    };

    std::optional<std::size_t> CountyOffsetCalculator::to_global_offset_impl(const std::string_view fips_code) const {
        const auto iter = _fips_map.left.find(std::string(fips_code));
        if (iter == _fips_map.left.end()) {
            spdlog::error("Cannot find offset for code {}", fips_code);
            return {};
        };

        return iter->second;
    };

    std::optional<std::string> CountyOffsetCalculator::from_global_offset_impl(const std::size_t fips_idx) const {
        const auto iter = _fips_map.right.find(fips_idx);
        if (iter == _fips_map.right.end()) {
            spdlog::error("Cannot find for index {}/{}", fips_idx, fips_idx);
            return {};
        };

        return iter->second;
    }

    std::size_t CountyOffsetCalculator::to_local_offset_impl(const std::string_view fips_code) const {
        const auto cbg_idx_opt = to_global_offset_impl(fips_code);
        if (!cbg_idx_opt.has_value()) {
            throw std::invalid_argument("Offset is out of bounds");
        }
        if (*cbg_idx_opt < _start_idx || *cbg_idx_opt > _end_idx) {
            throw std::invalid_argument("Index is out of bounds");
        }
        return *cbg_idx_opt - _start_idx;
    };

    std::optional<std::string> CountyOffsetCalculator::from_local_offset_impl(const size_t fips_idx) const {
        const auto local_offset = fips_idx + _start_idx;
        return from_global_offset_impl(local_offset);
    };



}