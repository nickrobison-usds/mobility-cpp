//
// Created by Nicholas Robison on 7/29/20.
//

#include "SafegraphMapper.hpp"

#include <shared/TileConfiguration.hpp>

void SafegraphMapper::setup(const mt::ctx::MapContext<data_row, mt::coordinates::Coordinate3D> &ctx) {

}

void SafegraphMapper::map(const mt::ctx::MapContext<data_row, mt::coordinates::Coordinate3D> &ctx,
                          const std::string &info) const {
    const auto l = info.size();
};
