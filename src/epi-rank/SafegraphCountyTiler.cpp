//
// Created by Nicholas Robison on 9/14/20.
//

#include "SafegraphCountyTiler.hpp"
#include <Eigen/Core>
#include <shared/ConversionUtils.hpp>
#include <shared/DateUtils.hpp>
#include <Spectra/GenEigsSolver.h>

void SafegraphCountyTiler::receive(const mt::ctx::ReduceContext<county_visit, mt::coordinates::Coordinate3D> &ctx,
                                   const mt::coordinates::Coordinate3D &key, const county_visit &value) {
    // Convert back to local coordinate space
    const auto local_temporal = value.visit_date - date::days{_tc._time_offset};
    const auto x_idx = _oc->to_local_offset(value.location_fips);
    const auto y_idx = _oc->to_global_offset(value.visit_fips);
    const auto l_time_count = local_temporal.time_since_epoch().count();
    if (y_idx.has_value()) {
        _matricies->insert(l_time_count, x_idx, *y_idx, value.visits);
    } else {
        spdlog::error("Visitor FIPS {} is not in map, skipping insert", value.visit_fips);
    }

}

void SafegraphCountyTiler::compute(const mt::ctx::ReduceContext<county_visit, mt::coordinates::Coordinate3D> &ctx) {
    // Compute some eigen values
    spdlog::debug("Computing centrality");
    for(std::size_t i = 0; i < _tc._time_count; i++) {
        auto A = _matricies->get_matrix_pair(i);
        auto M = A + A.transpose();

        Spectra::DenseGenMatProd<double> op(M);
        Spectra::GenEigsSolver<double, Spectra::LARGEST_MAGN, Spectra::DenseGenMatProd<double>> eigs(&op, 3, 6);

        eigs.init();
        int nconv = eigs.compute();

        Eigen::VectorXcd evalues;
        if (eigs.info() == Spectra::SUCCESSFUL){
            spdlog::debug("Found eigen values");
            evalues = eigs.eigenvalues();
        } else {
            spdlog::error("Cannot compute eigen vectors: {}", eigs.info());
        }

        spdlog::info("Eigenvalues: {}", evalues);

    }
}



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
    _start_date = d;

    _tc._nr = 16;
    // These values are really confusing
    _tc._time_offset = (d + date::days{ctx.get_tile().min_corner().get_dim0()}).time_since_epoch().count();
    _tc._time_count = ctx.get_tile().max_corner().get_dim0() - ctx.get_tile().min_corner().get_dim0();
    _tc._tile_min = ctx.get_tile().min_corner().get_dim1();
    _tc._tile_max = ctx.get_tile().max_corner().get_dim1();

    const auto loc_dims = ctx.get_tile().max_corner().get_dim1() - ctx.get_tile().min_corner().get_dim1();
    const auto visit_dims = ctx.get_tile().max_corner().get_dim2() - ctx.get_tile().min_corner().get_dim2();

    _c_wrapper = std::make_unique<components::CountyShapefileWrapper>(*county_path);
    _oc = std::make_unique<components::detail::CountyOffsetCalculator>(_c_wrapper->build_offsets().get(), _tc);
    _matricies = std::make_unique<EigenMatricies>(_tc._time_count, loc_dims, visit_dims);
}
