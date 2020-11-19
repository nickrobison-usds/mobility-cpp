//
// Created by Nicholas Robison on 7/29/20.
//

#include "SafegraphCBGTiler.hpp"
#include <boost/filesystem.hpp>
#include <boost/range/irange.hpp>
#include <blaze/math/CompressedVector.h>
#include <blaze/math/DynamicVector.h>
#include <hpx/parallel/algorithms/transform_reduce.hpp>
#include <hpx/parallel/algorithms/for_each.hpp>
#include <components/TileWriter2.hpp>
#include <components/VisitMatrixWriter2.hpp>
#include <shared/DateUtils.hpp>
#include <shared/ConversionUtils.hpp>
#include <spdlog/fmt/fmt.h>
#include <spdlog/spdlog.h>
#include <algorithm>

namespace fs = boost::filesystem;
namespace par = hpx::parallel;

void print_timing(const std::string &op_name, const std::uint64_t elapsed) {
    const chrono::nanoseconds n{elapsed};
    spdlog::debug("[{}] took {} ms", op_name, chrono::duration_cast<chrono::milliseconds>(n).count());
}

struct CentralityComputation {

    std::vector<cbg_centrality> operator()(const int date) const {
        auto compute_start = hpx::util::high_resolution_clock::now();
        auto res = _graphs->calculate_degree_centrality(date);
        const auto compute_elapsed = hpx::util::high_resolution_clock::now() - compute_start;
        print_timing(fmt::format("Compute for date {}", date), compute_elapsed);

        // Transform it into centrality measures
        std::vector<cbg_centrality> transformed;
        transformed.reserve(res.size());
        std::transform(res.begin(), res.end(), std::back_inserter(transformed), [this, &date](const auto &r) {
            const date::sys_days centrality_date = _start_date + date::days{date};
            return cbg_centrality{DEGREE, r.first, centrality_date, r.second};
        });

        return transformed;
    }

    components::TemporalGraphs *_graphs;
    const date::sys_days _start_date;
};

void SafegraphCBGTiler::setup(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx) {
    const auto cbg_path = ctx.get_config_value("cbg_path");
    const auto poi_path = ctx.get_config_value("poi_path");
    const auto start_date_string = ctx.get_config_value("start_date");
    const shared::days start_date(shared::ConversionUtils::convert_empty<std::uint64_t>(*start_date_string));
    const auto end_date_string = ctx.get_config_value("end_date");
    const shared::days end_date(shared::ConversionUtils::convert_empty<std::uint64_t>(*end_date_string));

    const auto sd = chrono::floor<date::days>(start_date);

    // Force cast to sys_days
    const date::sys_days d(start_date);
    const date::sys_days e(end_date);
    _start_date = d;

    _s = std::make_unique<components::CBGShapefileWrapper>(components::CBGShapefileWrapper(*cbg_path));

    _tc._nr = 16;
    // These values are really confusing
    _tc._time_offset = (d + date::days{ctx.get_tile().min_corner().get_dim0()}).time_since_epoch().count();
    _tc._time_count = ctx.get_tile().max_corner().get_dim0() - ctx.get_tile().min_corner().get_dim0();
    _tc._tile_min = ctx.get_tile().min_corner().get_dim1();
    _tc._tile_max = ctx.get_tile().max_corner().get_dim1();
    _oc = std::make_unique<components::detail::CBGOffsetCalculator>(
            components::detail::CBGOffsetCalculator(_s->build_offsets().get(), _tc));

    const auto loc_dims = ctx.get_tile().max_corner().get_dim1() - ctx.get_tile().min_corner().get_dim1();
    const auto visit_dims = ctx.get_tile().max_corner().get_dim2() - ctx.get_tile().min_corner().get_dim2();

    // Initialize the Temporal Matricies
    _tm = std::make_unique<components::TemporalMatricies>(_tc._time_count, loc_dims, visit_dims);
    _graphs = std::make_unique<components::TemporalGraphs>(_tc._time_count);
    _staging = std::vector<std::vector<v2>>(_tc._tile_max - _tc._tile_min);
    spdlog::debug("Tiler setup complete.");
}

void SafegraphCBGTiler::receive(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx,
                                const mt::coordinates::Coordinate3D &key,
                                const v2 &value) {
    // Convert back to local coordinate space
    const auto local_temporal = value.visit_date - date::days{_tc._time_offset};
    const auto y_idx = _oc->to_global_offset(value.visit_cbg);
    const auto l_time_count = local_temporal.time_since_epoch().count();
    if (y_idx.has_value()) {
        // Insert into the staging array
        std::lock_guard<std::mutex> l(_m);
        _staging.at(l_time_count).push_back(value);
    } else {
        spdlog::error("Visitor CBG {} is not in map, skipping insert", value.visit_cbg);
    }
}

void SafegraphCBGTiler::compute(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx) {
    populate_graph(ctx);
    write_parquet(ctx);
}

void SafegraphCBGTiler::populate_graph(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx) {
    // Do the insert asynchronously. We don't need a lock here, because each thread only touches a single graph/matrix
    std::vector<hpx::future<void>> results;
    for (std::size_t i = 0; i < _tc._tile_max - _tc._tile_min; i++) {
        auto f = hpx::async([this, i]() {
            const auto values = _staging.at(i);
//            FIXME(nickrobison): This is still horribly inefficient. But at least the inefficiency is amortized over all the nodes, rather just with the mappers.
            std::for_each(values.begin(), values.end(), [this, i](const auto &value) {
                // Convert back to local coordinate space
                const auto x_idx = _oc->to_local_offset(value.location_cbg);
                const auto y_idx = _oc->to_global_offset(value.visit_cbg);
                _tm->insert(i, x_idx, *y_idx, value.visits, value.distance);
//                Insert into the graph
                _graphs->insert(i, value);
            });
        });
        results.push_back(std::move(f));
    }

    return hpx::wait_all(results);
}

void SafegraphCBGTiler::write_parquet(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx) const {
    const auto output_name = ctx.get_config_value("output_name");
    const auto output_dir = ctx.get_config_value("output_dir");

    // Figure out my temporal start date
    const date::sys_days start_date = date::sys_days{} + date::days(_tc._time_offset);
    const date::sys_days end_date = start_date + date::days(_tc._time_count);
    const auto date_range = boost::irange(0, static_cast<int>(_tc._time_count));


    const auto parquet_filename = fmt::format("{}-{}-{}-{}-{}.parquet",
                                              *output_name,
                                              *(_oc->from_global_offset(_tc._tile_min)),
                                              *(_oc->from_global_offset(_tc._tile_max)),
                                              date::format("%F", start_date),
                                              date::format("%F", end_date));
    const auto p_file = fs::path(*output_dir) /= fs::path(parquet_filename);

    components::TileWriter2 tw(std::string(p_file.string()), *_oc);

    const auto visit_filename = fmt::format("{}-visits-{}-{}-{}-{}.parquet",
                                            *output_name,
                                            *(_oc->from_global_offset(_tc._tile_min)),
                                            *(_oc->from_global_offset(_tc._tile_max)),
                                            date::format("%F", start_date),
                                            date::format("%F", end_date));

    const auto v_file = fs::path(*output_dir) /= fs::path(visit_filename);


    components::VisitMatrixWriter2 vw(std::string(v_file.string()), *_oc);

    par::for_each(par::execution::par_unseq, date_range.begin(), date_range.end(), [&](const std::size_t i) {
        // Some nice pretty-printing of the dates
        const date::sys_days matrix_date = start_date + date::days{i};

        const auto multiply_start = hpx::util::high_resolution_clock::now();
        const auto matrix_pair = _tm->get_matrix_pair(i);
        const distance_matrix result = _tm->compute(i);

        // Sum the total risk for each cbg
        const blaze::CompressedVector<double, blaze::rowVector> cbg_risk_score = blaze::sum<blaze::columnwise>(
                result);
        const double max = blaze::max(cbg_risk_score);
        const blaze::CompressedVector<std::uint32_t, blaze::rowVector> visit_sum = blaze::sum<blaze::columnwise>(
                matrix_pair.vm);

        // scale it back down
        spdlog::info("Performing multiplication for {}", date::format("%F", matrix_date));
        const auto multiply_elapsed = hpx::util::high_resolution_clock::now() - multiply_start;
        print_timing("Multiply", multiply_elapsed);
        arrow::Status status = tw.writeResults(matrix_date, cbg_risk_score, {}, visit_sum);
        if (!status.ok()) {
            spdlog::critical("Could not write parquet file: {}", status.CodeAsString());
        }
        status = vw.writeResults(matrix_date, matrix_pair.vm);
        if (!status.ok()) {
            spdlog::critical("Could not write parquet file: {}", status.CodeAsString());
        }

    });
    const auto write_start = hpx::util::high_resolution_clock::now();
    spdlog::info("Beginning tile write");
    const auto write_elapsed = hpx::util::high_resolution_clock::now() - write_start;
    print_timing("File Write", write_elapsed);
    // Write the files
    arrow::Status status;
    status = tw.writeToDisk();
    if (!status.ok()) {
        spdlog::error("Error writing tiles");
    }
    status = vw.writeToDisk();
    if (!status.ok()) {
        spdlog::error("Error visit patterns");
    }

}

std::vector<cbg_centrality>
SafegraphCBGTiler::reduce(const mt::ctx::ReduceContext<v2, mt::coordinates::Coordinate3D> &ctx) const {
    typedef std::vector<cbg_centrality> reduce_type;

    // Iterate over all the dates and compute the values for the dates
    const auto date_range = boost::irange(0, static_cast<int>(_tc._time_count));
    return par::transform_reduce(par::execution::par_unseq, date_range.begin(), date_range.end(),
                                 reduce_type(),
                                 [](reduce_type acc, reduce_type v) {
                                     acc.reserve(acc.size() + v.size());
                                     move(v.begin(), v.end(), back_inserter(acc));
                                     return acc;
                                 }, CentralityComputation{_graphs.get(), _start_date});
}
