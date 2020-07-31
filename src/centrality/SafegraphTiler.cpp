//
// Created by Nicholas Robison on 7/29/20.
//

#include "SafegraphTiler.hpp"
#include <spdlog/spdlog.h>

void SafegraphTiler::receive(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx,
                             const mt::coordinates::Coordinate3D &key,
                             const v2 &value) {
    spdlog::debug("I have it!");
}

void SafegraphTiler::compute() {

}