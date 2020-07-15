//
// Created by Nicholas Robison on 6/1/20.
//

#include "TileServer.hpp"
#include "../OffsetCalculator.hpp"
#include "../RowProcessor.hpp"
#include "../TileWriter.hpp"
#include "../VisitMatrixWriter.hpp"
#include "../vector_scaler.hpp"
#include <absl/strings/str_split.h>
#include <blaze/math/Math.h>
#include <blaze/math/CompressedVector.h>
#include <blaze/math/DynamicVector.h>
#include <boost/filesystem.hpp>
#include <boost/regex.hpp>
#include <hpx/parallel/execution.hpp>
#include <hpx/parallel/algorithms/transform.hpp>
#include <io/csv_reader.hpp>
#include "spdlog/spdlog.h"

#include <algorithm>
#include <utility>

namespace fs = boost::filesystem;

namespace components::server {

    void print_timing(const std::string &op_name, const std::uint64_t elapsed) {
        const chrono::nanoseconds n{elapsed};
        spdlog::debug("[{}] took {} ms", op_name, chrono::duration_cast<chrono::milliseconds>(n).count());
    }

    std::vector<weekly_pattern> extract_rows(const string &filename) {

        // Get date from filename
        spdlog::info("Reading {}", filename);
        io::CSVLoader<17, true> l(filename);

        string safegraph_place_id;
        string location_name;
        string street_address;
        string city;
        string region;
        string postal_code;
        string iso_country_code;
        string safegraph_brand_ids;
        string brands;
        string date_range_start;
        string date_range_end;
        uint32_t raw_visit_counts;
        uint32_t raw_visitor_counts;
        string visits_by_day;
        string visits_by_each_hour;
        uint64_t poi_cbg;
        string visitor_home_cbgs;

        return l.read<weekly_pattern>(
                [](const string &safegraph_place_id,
                   const string &location_name,
                   const string &street_address,
                   const string &city,
                   const string &region,
                   const string &postal_code,
                   const string &iso_country_code,
                   const string &safegraph_brand_ids,
                   const string &brands,
                   const string &date_range_start,
                   const string &date_range_end,
                   const uint32_t raw_visit_counts,
                   const uint32_t raw_visitor_counts,
                   const string visits_by_day,
                   const string visits_by_each_hour,
                   const uint64_t poi_cbg,
                   const string &visitor_home_cbgs) {

                    // Parse the start/end dates
                    std::istringstream start{date_range_start};
                    std::istringstream end{date_range_end};
                    date::sys_days start_date;
                    date::sys_days end_date;
                    start >> date::parse("%F", start_date);
                    end >> date::parse("%F", end_date);


                    weekly_pattern loc{safegraph_place_id,
                                       location_name,
                                       start_date,
                                       end_date,
                                       raw_visit_counts,
                                       raw_visitor_counts,
                                       visits_by_day,
                                       visits_by_each_hour,
                                       poi_cbg,
                                       visitor_home_cbgs};

                    return loc;
                }, safegraph_place_id,
                location_name,
                street_address,
                city,
                region,
                postal_code,
                iso_country_code,
                safegraph_brand_ids,
                brands,
                date_range_start,
                date_range_end,
                raw_visit_counts,
                raw_visitor_counts,
                visits_by_day,
                visits_by_each_hour,
                poi_cbg,
                visitor_home_cbgs);
    }

    TileServer::TileServer(const std::string output_dir, const std::string output_name, const std::string cbg_shp,
                           const std::string poi_parquet) : _output_dir(std::move(output_dir)),
                                                            _output_name(std::move(output_name)),
                                                            _l({}, poi_parquet, poi_parquet),
                                                            _s(cbg_shp) {
        // Not used
    };

    void TileServer::init(const TileConfiguration &dim, std::size_t num_nodes) {

        // Read the CSV file
        const auto rows = extract_rows(dim._filename);
        const date::sys_days start_date = date::sys_days{} + date::days(dim._time_offset);

        // TODO: This should be where we do async initialization
        // Build the CBG offsetmap
        const auto of = _s.build_offsets().get();
        detail::OffsetCalculator offset_calculator(of, dim);

        std::vector<hpx::future<void>> results;
        results.reserve(rows.size());

        // Create the Row Processor
        RowProcessor processor{dim, _l, _s, offset_calculator, start_date};

        // Semaphore for limiting the number of rows to process concurrently.
        // This should help make sure we make progress across all the threads
        spdlog::debug("Processing {} rows concurrently", dim._nr);
        hpx::lcos::local::sliding_semaphore sem(dim._nr);
        for (std::size_t t = 0; t < rows.size(); t++) {
            const auto row = std::make_shared<weekly_pattern>(rows.at(t));
            auto res = processor.process_row(row).then([&sem, t](hpx::future<void> f) {
                sem.signal(t);
            });

            results.push_back(std::move(res));
            const auto sem_start = hpx::util::high_resolution_clock::now();
            spdlog::debug("Waiting on semaphore");
            sem.wait(t);
            const auto sem_elapsed = hpx::util::high_resolution_clock::now() - sem_start;
            print_timing("Semaphore Wait", sem_elapsed);

        };
        // When each result is completed, load it into the distance and visit matricies
        spdlog::debug("Waiting for {} rows", results.size());
        hpx::wait_all(results);

        // Create the HDF5 file
        std::array<hsize_t, 3> dims{7, MAX_CBG, MAX_CBG};
        const auto hdf5_filename = fmt::format("{}", "connectivity-graph.hdf5");
        const auto h_file = fs::path(_output_dir) /= fs::path(hdf5_filename);
        io::HDF5<connectivity_output, 3> shared_file(h_file.string(), "connectivity_graph", dims);

        // Now, multiply the values and write them to disk
        for (uint i = 0; i < dim._time_count; i++) {
            // Some nice pretty-printing of the dates
            const date::sys_days matrix_date = start_date + date::days{i};
            const auto parquet_filename = fmt::format("{}-{}-{}-{}-{}.parquet", hpx::get_locality_id(),
                                                      date::format("%F", matrix_date), dim._cbg_min, dim._cbg_max,
                                                      _output_name);
            const auto p_file = fs::path(_output_dir) /= fs::path(parquet_filename);

            const auto visit_filename = fmt::format("{}-{}-{}-{}-visits-{}.parquet", hpx::get_locality_id(),
                                                    date::format("%F", matrix_date), dim._cbg_min, dim._cbg_max,
                                                    _output_name);

            const auto v_file = fs::path(_output_dir) /= fs::path(visit_filename);

            TileWriter tw(std::string(p_file.string()), offset_calculator);
            VisitMatrixWriter vw(std::string(v_file.string()), offset_calculator);

            const auto multiply_start = hpx::util::high_resolution_clock::now();
            TemporalMatricies &matricies = processor.get_matricies();
            const auto matrix_pair = matricies.get_matrix_pair(i);
            const distance_matrix result = matricies.compute(i);

            // Sum the total risk for each cbg
            const blaze::CompressedVector<double, blaze::rowVector> cbg_risk_score = blaze::sum<blaze::columnwise>(
                    result);
            const double max = blaze::max(cbg_risk_score);
            const blaze::CompressedVector<std::uint32_t, blaze::rowVector> visit_sum = blaze::sum<blaze::columnwise>(
                    matrix_pair.vm);

            // scale it back down
            spdlog::info("Performing multiplication for {}", date::format("%F", matrix_date));
            const auto scaled_results = blaze::map(cbg_risk_score, detail::VectorScaler<double>(max));
            const auto multiply_elapsed = hpx::util::high_resolution_clock::now() - multiply_start;
            print_timing("Multiply", multiply_elapsed);

            spdlog::info("Beginning tile write");
            const auto write_start = hpx::util::high_resolution_clock::now();

            // Write out to the HDF5 file
            for (std::size_t hi = 0; hi < matrix_pair.vm.columns(); hi++) {
                const auto poi_cbg = offset_calculator.cbg_from_local_offset(hi);
                if (!poi_cbg.has_value()) {
                    spdlog::error("Cannot process source cbg: `{}`", hi);
                    continue;
                }

                std::vector<connectivity_output> results_to_write(MAX_CBG);
                for (auto pair = std::make_pair(matrix_pair.vm.cbegin(hi), matrix_pair.dm.cbegin(hi));
                     pair.first != matrix_pair.vm.cend(hi); ++pair.first, ++pair.second) {
                    const auto vm_dist = std::distance(matrix_pair.vm.cbegin(hi), pair.first);
                    const auto dm_dist = std::distance(matrix_pair.dm.cbegin(hi), pair.second);
                    const auto visitor_cbg = offset_calculator.cbg_from_offset(vm_dist);
                    if (!visitor_cbg.has_value()) {
                        spdlog::error("Cannot process dest cbg: `{}`", vm_dist);
                        continue;
                    }
                    auto pc2 = *poi_cbg->c_str();
                    auto vc2 = *visitor_cbg->c_str();
                    connectivity_output o{&pc2, &vc2, pair.first->value(), pair.second->value(), 0.0};
                    results_to_write.at(vm_dist) = o;
                }

                std::array<hsize_t, 3> count{1, 1, MAX_CBG};
                std::array<hsize_t, 3> offset{i, dim._cbg_min + hi, 0};
                // Write it out
                shared_file.write(count, offset, results_to_write);
            }
            const auto write_elapsed = hpx::util::high_resolution_clock::now() - write_start;
            print_timing("File Write", write_elapsed);

//            arrow::Status status = tw.writeResults(start_date, cbg_risk_score, scaled_results, visit_sum);
//            if (!status.ok()) {
//                spdlog::critical("Could not write parquet file: {}", status.CodeAsString());
//            }
//            status = vw.writeResults(start_date, matrix_pair.vm);
//            if (!status.ok()) {
//                spdlog::critical("Could not write parquet file: {}", status.CodeAsString());
//            }

        }

        spdlog::info("Tile computation complete");
    }
}
