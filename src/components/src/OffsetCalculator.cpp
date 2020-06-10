//
// Created by Nicholas Robison on 6/10/20.
//

#include "OffsetCalculator.hpp"

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

    std::size_t OffsetCalculator::calculate_cbg_offset(const std::string &cbg_code) const {
        return _cbg_map.left.at(cbg_code);
    };

    std::size_t OffsetCalculator::calculate_local_offset(const std::string &cbg_code) const {
        const auto cbg_idx = calculate_cbg_offset(cbg_code);
        if (cbg_idx < _start_idx || cbg_idx > _end_idx) {
            throw std::invalid_argument("Index is out of bounds");
        }
        return cbg_idx - _start_idx;
    };

    std::string OffsetCalculator::cbg_from_local_offset(const size_t cbg_idx) const {
        return _cbg_map.right.at(cbg_idx + _start_idx);
    };
}