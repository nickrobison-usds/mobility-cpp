//
// Created by Nicholas Robison on 7/29/20.
//

#include "SafegraphMapper.hpp"
#include <shared/TileConfiguration.hpp>
#include <shared/DateUtils.hpp>
#include <shared/ConversionUtils.hpp>
#include <shared/QuotedLineSplitter.hpp>
#include <absl/strings/str_split.h>
#include <boost/regex.hpp>
#include <string_view>

// Duplicate a bunch of code, just to get things compiling

static const boost::regex brackets(R"(\[|\]|")");
static const boost::regex cbg_map_replace("{|\"|}");

size_t compute_temporal_offset(const date::sys_days &start_date, const date::sys_days &row_date) {
    const auto diff = row_date - start_date;
    return std::abs(diff.count());
}

std::vector<v2>
compute_distance(const std::shared_ptr<joined_location> loc, const std::vector<v2> &patterns,
                 const absl::flat_hash_map<std::string, OGRPoint> &centroids) {
    spdlog::debug("Calculating distances for {}", loc->safegraph_place_id);
    const OGRPoint loc_point(loc->longitude, loc->latitude);
    patterns.size();
    std::vector<v2> o;
    o.reserve(patterns.size());
    std::transform(patterns.begin(), patterns.end(), std::back_inserter(o),
                   [&centroids, &loc_point, &loc](auto &row) {
                       const auto cbg_centroid = centroids.at(row.visit_cbg);
                       const auto distance = loc_point.Distance(&cbg_centroid);
                       v2 r2{row.safegraph_place_id, row.visit_date, loc->location_cbg,
                             row.visit_cbg, row.visits, distance, 0.0F};
                       return r2;
                   });
    spdlog::debug("Finished calculating distances for {}", loc->safegraph_place_id);
    return o;
}

std::vector<std::pair<std::string, std::uint16_t>> extract_cbg_visits(const weekly_pattern &row) {
    // Extract the CBGs which get visited
    const auto cbg_replaced = boost::regex_replace(row.visitor_home_cbgs, cbg_map_replace, "");
    // Split into key/pairs, then split the pairs
    const auto visit_pairs = absl::StrSplit(cbg_replaced, ',');
    std::vector<std::pair<std::string, std::uint16_t>> cbg_visits;
    std::transform(visit_pairs.begin(), visit_pairs.end(), std::back_inserter(cbg_visits), [](const auto &kv_pair) {
        const std::pair<std::string, std::string> split_pair = absl::StrSplit(kv_pair, ':');
        std::uint16_t v = 0;
        // If the value is empty, then we don't even need to try for the stoi. And we avoid throwing/catching the exception
        if (!split_pair.second.empty()) {
            try {
                v = std::stoi(split_pair.second);
            } catch (std::invalid_argument &e) {
                spdlog::error("Cannot convert {} for {}", split_pair.second, split_pair.first);
            }
        }
        return std::make_pair(split_pair.first, v);
    });

    std::vector<std::pair<std::string, std::uint16_t>> filtered;
    std::copy_if(cbg_visits.begin(), cbg_visits.end(), std::back_inserter(filtered), [](const auto &v) {
        return !v.first.empty();
    });

    return filtered;
}

std::vector<v2>
expandRow(const weekly_pattern &row,
          const std::vector<std::pair<std::string, std::uint16_t>> &cbg_visits) {
    // Extract the number of visits each day
    const auto replaced = boost::regex_replace(row.visits_by_day, brackets, std::string(""));
    // Return a vector of string, because stoi doesn't support string_view.
    const std::vector<std::string> split_visits = absl::StrSplit(replaced, ',');
    std::vector<std::uint16_t> visits;
    std::transform(split_visits.begin(), split_visits.end(), std::back_inserter(visits), [](const auto &tr) {
        std::uint16_t v = 0;
        try {
            v = std::stoi(tr);
        } catch (std::invalid_argument &e) {
            spdlog::error("No conversion for {}", tr);
        }
        return v;
    });

    std::vector<v2> output;
    const std::size_t r = visits.size() * cbg_visits.size();
    spdlog::debug("Expecting {} rows", r);
    output.reserve(r);

    // Iterate through both sets and generate a v2 struct for each day/cbg pair
    for (int i = 0; i < visits.size(); i++) {
        const auto visit = visits[i];
        std::for_each(cbg_visits.begin(), cbg_visits.end(), [&row, &output, &visit, i](const auto &cbg_pair) {
            const v2 day{row.safegraph_place_id, row.date_range_start + date::days{i}, "", cbg_pair.first, visit,
                         0.0F, 0.0F};
            output.push_back(day);
        });
    }
    return output;
}

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

void SafegraphMapper::setup(const mt::ctx::MapContext<v2, mt::coordinates::Coordinate3D> &ctx) {
    const auto cbg_path = ctx.get_config_value("cbg_path");
    const auto poi_path = ctx.get_config_value("poi_path");
    const auto start_date_string = ctx.get_config_value("start_date");
    const shared::days start_date(shared::ConversionUtils::convert_empty<std::uint64_t>(*start_date_string));
    const auto end_date_string = ctx.get_config_value("end_date");
    const shared::days end_date(shared::ConversionUtils::convert_empty<std::uint64_t>(*end_date_string));

    const auto sd = chrono::floor<date::days>(start_date);


    // Force cast to sys_days
    const auto ff = chrono::floor<date::days>(start_date);
    _begin_date = date::sys_days(start_date);
    _end_date = date::sys_days(end_date);


    const auto time_bounds = chrono::duration_cast<shared::days>(end_date - start_date).count();
//
    _l = std::make_unique<components::JoinedLocation>(components::JoinedLocation({}, *poi_path, *poi_path));
    _s = std::make_unique<components::ShapefileWrapper>(components::ShapefileWrapper(*cbg_path));

    _tc._nr = 16;
    // These values are really confusing
    _tc._time_offset = (_begin_date + date::days{ctx.get_tile().min_corner().get_dim0()}).time_since_epoch().count();
    _tc._time_count = time_bounds;
    _tc._cbg_min = ctx.get_tile().min_corner().get_dim1();
    _tc._cbg_max = ctx.get_tile().max_corner().get_dim1();
    _oc = std::make_unique<components::detail::OffsetCalculator>(
            components::detail::OffsetCalculator(_s->build_offsets().get(), _tc));

    spdlog::debug("Mapper setup complete.");
}

void SafegraphMapper::map(const mt::ctx::MapContext<v2, mt::coordinates::Coordinate3D> &ctx,
                          const std::string &info) const {
    const auto row = parse_string(info);

    if (row.date_range_start < _begin_date || row.date_range_end > _end_date)
        return;

    const auto rows = process_row(row);

    std::for_each(rows.begin(), rows.end(), [&ctx, this](const auto &r) {
        const auto global_temporal = r.visit_date - date::days{this->_tc._time_offset};
        const auto g_count = global_temporal.time_since_epoch().count();
        const auto loc_offset = this->_oc->calculate_cbg_offset(r.location_cbg);
        const auto visit_offset = this->_oc->calculate_cbg_offset(r.visit_cbg);
        if (visit_offset.has_value() && loc_offset.has_value() && g_count >= 0) {
            ctx.emit(mt::coordinates::Coordinate3D(g_count, *loc_offset, *visit_offset), r);
        }
    });
}

std::vector<v2> SafegraphMapper::process_row(const weekly_pattern &row) const {
    return _l->find_location(row.safegraph_place_id).then(
            [this, row](hpx::future<joined_location> location_future) {
                const auto jl = std::make_shared<joined_location>(location_future.get());
                if (jl->location_cbg.empty()) {
                    spdlog::warn("Cannot find CBG for safegraph place: {}", row.safegraph_place_id);
                    const std::vector<v2> f;
                    return hpx::make_ready_future<std::vector<v2>>(f);
                } else {
                    return hpx::make_ready_future<std::vector<v2>>(handle_row(row, jl));
                }

            }).get();
}

std::vector<v2>
SafegraphMapper::handle_row(const weekly_pattern &row, const std::shared_ptr<joined_location> &jl) const {
    const auto visits = extract_cbg_visits(row);
    auto centroid_future = get_centroid_map(visits);
    std::vector<v2> row_expanded = expandRow(row, visits);
    return compute_distance(jl, row_expanded, centroid_future);
}

absl::flat_hash_map<std::string, OGRPoint>
SafegraphMapper::get_centroid_map(const std::vector<std::pair<std::string, std::uint16_t>> &visits) const {
    std::vector<std::string> cbgs;
    std::transform(visits.begin(), visits.end(), std::back_inserter(cbgs), [](const auto &pair) {
        return pair.first;
    });

    const auto centroids = _s->get_centroids(cbgs).get();

    return absl::flat_hash_map<std::string, OGRPoint>(centroids.begin(), centroids.end());
}
