//
// Created by Nicholas Robison on 7/29/20.
//

#include "SafegraphMapper.hpp"
#include <shared/TileConfiguration.hpp>
#include <shared/DateUtils.hpp>
#include <shared/ConversionUtils.hpp>
#include <shared/QuotedLineSplitter.hpp>
#include <string_view>

weekly_pattern parse_string(const std::string_view v) {
    const auto splits = shared::QuotedStringSplitter(v);

    weekly_pattern w{
        splits[0],
        splits[1],
        shared::DateUtils::to_days(splits[9]),
        shared::DateUtils::to_days(splits[10]),
        shared::ConversionUtils::convert_empty<uint32_t>(splits[11]),
        shared::ConversionUtils::convert_empty<uint32_t>(splits[11]),
        splits[13],
        splits[14],
        shared::ConversionUtils::convert_empty<uint64_t>(splits[15]),
        splits[16]
    };

    return w;
}

void SafegraphMapper::setup(const mt::ctx::MapContext<data_row, mt::coordinates::Coordinate3D> &ctx) {
    const auto cbg_path = ctx.get_config_value("cbg_path");
    const auto poi_path = ctx.get_config_value("poi_path");
    _l = std::make_unique<components::JoinedLocation>(components::JoinedLocation({}, *poi_path, *poi_path));
    _s = std::make_unique<components::ShapefileWrapper>(components::ShapefileWrapper(*cbg_path));

    // Create the row processor, at some point
}

void SafegraphMapper::map(const mt::ctx::MapContext<data_row, mt::coordinates::Coordinate3D> &ctx,
                          const std::string &info) const {
    const auto row = parse_string(info);
    row.safegraph_place_id;
};
