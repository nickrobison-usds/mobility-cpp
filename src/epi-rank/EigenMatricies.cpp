//
// Created by Nicholas Robison on 9/14/20.
//

#include "EigenMatricies.hpp"
#include <spdlog/spdlog.h>

std::vector<matrix_type>
initialize_matricies(const std::size_t matrix_count, const std::size_t col_dimension, const std::size_t row_dimension) {
    spdlog::debug("Allocating {} matricies of size (rows/cols) {}/{}", matrix_count, row_dimension, col_dimension);
    std::vector<matrix_type> matricies;
    matricies.reserve(matrix_count);
    for(std::size_t i = 0; i < matrix_count; i++) {
        matricies.emplace_back(row_dimension, col_dimension);
    }

    return matricies;
}

EigenMatricies::EigenMatricies(size_t matricies, size_t col_dimension, size_t row_dimension) : _matricies(
        initialize_matricies(matricies, col_dimension, row_dimension)), _locks(matricies) {
    // Not used
}

void EigenMatricies::insert(std::size_t time, std::size_t col, std::size_t row, std::uint16_t visits) {
    // TODO: This is incredibly inefficient. We should take the lock before we do the inserts
    absl::MutexLock l(&_locks.at(time));
    matrix_type &m = _matricies.at(time);
    m.coeffRef(row, col) += visits;
}


