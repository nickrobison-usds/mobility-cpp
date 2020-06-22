//
// Created by Nicholas Robison on 6/10/20.
//

#include "OffsetCalculator.hpp"
#include <spdlog/spdlog.h>

namespace components::detail {

    offset_bimap build_offset_map(const server::ShapefileServer::offset_type &offsets) {
        detail::offset_bimap cbg_offsets;

        std::for_each(offsets.begin(), offsets.end(), [&cbg_offsets](const auto &pair) {
            cbg_offsets.insert(detail::position(pair.first, pair.second));
        });

        return cbg_offsets;
    };

    OffsetCalculator::OffsetCalculator(const server::ShapefileServer::offset_type &init,
                                       const TileConfiguration &config) : _cbg_map(build_offset_map(init)),
                                                                          _start_idx(config._cbg_min),
                                                                          _end_idx(config._cbg_max) {
        // Not used
    };

    std::optional<std::size_t> OffsetCalculator::calculate_cbg_offset(const std::string &cbg_code) const {
        const auto iter = _cbg_map.left.find(cbg_code);
        if (iter == _cbg_map.left.end()) {
            spdlog::error("Cannot find offset for code {}", cbg_code);
            return {};
        };

        return iter->second;
    };

    std::optional<std::string> OffsetCalculator::cbg_from_offset(const std::size_t cbg_idx) const {
        const auto iter = _cbg_map.right.find(cbg_idx);
        if (iter == _cbg_map.right.end()) {
            spdlog::error("Cannot find for index {}/{}", cbg_idx, cbg_idx);
            return {};
        };

        return iter->second;
    }

    std::size_t OffsetCalculator::calculate_local_offset(const std::string &cbg_code) const {
        const auto cbg_idx_opt = calculate_cbg_offset(cbg_code);
        if (!cbg_idx_opt.has_value()) {
            throw std::invalid_argument("Offset is out of bounds");
        }
        if (*cbg_idx_opt < _start_idx || *cbg_idx_opt > _end_idx) {
            throw std::invalid_argument("Index is out of bounds");
        }
        return *cbg_idx_opt - _start_idx;
    };

    std::optional<std::string> OffsetCalculator::cbg_from_local_offset(const size_t cbg_idx) const {
        const auto local_offset = cbg_idx + _start_idx;
        return cbg_from_offset(local_offset);
    };
}