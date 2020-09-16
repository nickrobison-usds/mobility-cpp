//
// Created by Nicholas Robison on 9/14/20.
//

#include "SafegraphCountyTiler.hpp"
#include <blaze/math/dense/Eigen.h>
#include <boost/filesystem.hpp>
#include <io/parquet.hpp>
#include <shared/constants.hpp>
#include <shared/ConversionUtils.hpp>
#include <shared/DateUtils.hpp>

namespace fs = boost::filesystem;

void SafegraphCountyTiler::setup(const mt::ctx::ReduceContext<county_visit, mt::coordinates::Coordinate3D> &ctx) {
    const auto county_path = ctx.get_config_value("county_path");
    const auto poi_path = ctx.get_config_value("poi_path");
    const auto start_date_string = ctx.get_config_value("start_date");
    const shared::days start_date(shared::ConversionUtils::convert_empty<std::uint64_t>(*start_date_string));
    const auto end_date_string = ctx.get_config_value("end_date");
    const shared::days end_date(shared::ConversionUtils::convert_empty<std::uint64_t>(*end_date_string));

    const auto sd = chrono::floor<date::days>(start_date);

    // Force cast to sys_days
    const date::sys_days d(start_date);
    const date::sys_days e(end_date);


    _tc._nr = 16;
    // These values are really confusing
    _tc._time_offset = (d + date::days{ctx.get_tile().min_corner().get_dim0()}).time_since_epoch().count();
    _tc._time_count = ctx.get_tile().max_corner().get_dim0() - ctx.get_tile().min_corner().get_dim0();
    _tc._tile_min = ctx.get_tile().min_corner().get_dim1();
    _tc._tile_max = ctx.get_tile().max_corner().get_dim1();
    _start_date = d + date::days{_tc._time_offset};

    const auto loc_dims = ctx.get_tile().max_corner().get_dim1() - ctx.get_tile().min_corner().get_dim1();
    const auto visit_dims = ctx.get_tile().max_corner().get_dim2() - ctx.get_tile().min_corner().get_dim2();

    _c_wrapper = std::make_unique<components::CountyShapefileWrapper>(*county_path);
    _oc = std::make_unique<components::detail::CountyOffsetCalculator>(_c_wrapper->build_offsets().get(), _tc);
    matricies = std::make_unique<BlazeMatricies>(_tc._time_count, loc_dims, visit_dims);

    _output_path = *ctx.get_config_value("output_dir");
}

void SafegraphCountyTiler::receive(const mt::ctx::ReduceContext<county_visit, mt::coordinates::Coordinate3D> &ctx,
                                   const mt::coordinates::Coordinate3D &key, const county_visit &value) {
    // Convert back to local coordinate space
    const auto local_temporal = value.visit_date - date::days{_tc._time_offset};
    const auto x_idx = _oc->to_local_offset(value.location_fips);
    const auto y_idx = _oc->to_global_offset(value.visit_fips);
    const auto l_time_count = local_temporal.time_since_epoch().count();
    if (y_idx.has_value()) {
        matricies->insert(l_time_count, x_idx, *y_idx, value.visits);
    } else {
        spdlog::error("Visitor FIPS {} is not in map, skipping insert", value.visit_fips);
    }

}

void SafegraphCountyTiler::compute(const mt::ctx::ReduceContext<county_visit, mt::coordinates::Coordinate3D> &ctx) {
    // Compute some eigen values
    spdlog::debug("Computing eigenvalue centrality");
    // Figure out how many eigenvectors we need, based on the number of locations
    std::vector<hpx::future<void>> results;
    results.reserve(_tc._time_count);
    // Compute and write the results in parallel
    for (std::size_t i = 0; i < _tc._time_count; i++) {
        auto computation = hpx::async([this, i]() {
            auto B = matricies->get_matrix_pair(i);
            blaze::DynamicVector<complex<double>, blaze::columnVector>
                    w(_tc._tile_max - _tc._tile_min);   // The vector for the complex eigenvalues
            blaze::eigen(B, w);
            return w;
        }).then([this, i](auto f) {
            const auto values = f.get();
            // Write it out
            write_eigenvalues(i, values);
        });
        results.push_back(std::move(computation));
    }

    hpx::wait_all(results);
    spdlog::info("Computation is complete");
}

void SafegraphCountyTiler::write_eigenvalues(const std::size_t offset, const blaze::DynamicVector<complex<double>, blaze::columnVector> &values) const {

    // Some nice pretty-printing of the dates
    const date::sys_days matrix_date = date::sys_days(shared::days(_tc._time_offset)) + date::days{offset};
    const auto parquet_filename = fmt::format("{}-{}-{}-{}.parquet",
                                              "epi-rank",
                                              *_oc->from_global_offset(_tc._tile_min),
                                              *_oc->from_global_offset(_tc._tile_max),
                                              date::format("%F", matrix_date));
    const auto p_file = fs::path(_output_path) /= fs::path(parquet_filename);
    spdlog::info("Writing: {}", p_file);

    // Write out the CBGs by rank
    const io::Parquet p(p_file.string());
    arrow::StringBuilder _county_builder;
    arrow::Date32Builder _date_builder;
    arrow::DoubleBuilder _rank_builder;

    arrow::Status status;
    for(std::size_t i = 0; i < values.size(); i++) {
        const auto county = _oc->from_local_offset(i);
        status = _county_builder.Append(*county);
        status = _date_builder.Append(matrix_date.time_since_epoch().count());
        status = _rank_builder.Append(values[i].real());
    }

    std::shared_ptr<arrow::Array> county_array;
    status = _county_builder.Finish(&county_array);
    std::shared_ptr<arrow::Array> date_array;
    status = _date_builder.Finish(&date_array);
    std::shared_ptr<arrow::Array> rank_array;
    status = _rank_builder.Finish(&rank_array);

    auto schema = arrow::schema(
            {arrow::field("county", arrow::utf8()),
             arrow::field("date", arrow::date32()),
             arrow::field("rank", arrow::float64())
            });

    auto table = arrow::Table::Make(schema, {county_array, date_array, rank_array});
    status = p.write(*table);
}
