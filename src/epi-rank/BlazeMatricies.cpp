//
// Created by Nicholas Robison on 9/15/20.
//

#include "BlazeMatricies.hpp"
#include <spdlog/spdlog.h>

std::vector<BlazeMatricies::matrix_type>
initializeMatricies(const size_t matricies, const size_t col_dimension, const size_t row_dimension) {
    spdlog::debug("Allocating {} matricies of size (rows/cols) {}/{}", matricies, row_dimension, col_dimension);
    std::vector<BlazeMatricies::matrix_type> pairs;
    pairs.reserve(matricies);
    for (std::size_t i = 0; i < matricies; i++) {
        pairs.emplace_back(row_dimension, col_dimension, 0);
    }

    return pairs;
}

BlazeMatricies::BlazeMatricies(size_t matricies, size_t col_dimension, size_t row_dimension): _locks(matricies),
                                                                                              matricies(initializeMatricies(
                                                                                                      matricies,
                                                                                                      col_dimension,
                                                                                                      row_dimension)) {}

void BlazeMatricies::insert(std::size_t time, std::size_t col, std::size_t row, double visits) {
    // TODO: This is incredibly inefficient. We should take the lock before we do the inserts
    absl::MutexLock l(&_locks.at(time));
    auto &m = matricies.at(time);
    auto &ref = m(row, col);
    ref = ref + visits;
}

BlazeMatricies::matrix_type &BlazeMatricies::get_matrix_pair(std::size_t i) {
    return matricies.at(i);
}
