//
// Created by Nicholas Robison on 6/4/20.
//

#include "TemporalMatricies.hpp"
#include "spdlog/spdlog.h"

namespace components {

    std::vector<MatrixPair>
    initializeMatricies(const size_t matricies, const size_t col_dimension, const size_t row_dimension) {
        spdlog::debug("Allocating {} matricies of size (rows/cols) {}/{}", matricies, row_dimension, col_dimension);
        std::vector<MatrixPair> pairs;
        pairs.reserve(matricies);
        for (int i = 0; i < matricies; i++) {
            pairs.emplace_back(
                    visit_matrix(row_dimension, col_dimension, 0),
                    distance_matrix(row_dimension, col_dimension, 0)
            );
        }

        return pairs;
    }

    TemporalMatricies::TemporalMatricies(const size_t matricies, const size_t col_dimension, const size_t row_dimension)
            : _locks(matricies),
              matricies(initializeMatricies(
                      matricies,
                      col_dimension,
                      row_dimension)) {}

    void
    TemporalMatricies::insert(std::size_t time, std::size_t col, std::size_t row, std::uint16_t visits, double distance) {
        // TODO: This is incredibly inefficient. We should take the lock before we do the inserts
        auto &lock = _locks.at(time);
        lock.Lock();
        MatrixPair &pair = matricies.at(time);
        const std::uint32_t v2 = pair.vm.at(row, col);
        const double d2 = pair.dm.at(row, col);
        pair.vm.set(row, col, visits + v2);
        pair.dm.set(row, col,distance + d2);
        lock.Unlock();
    };

    distance_matrix TemporalMatricies::compute(const std::size_t i) {
        auto &lock = _locks.at(i);
        lock.Lock();
        MatrixPair &pair = matricies.at(i);
        distance_matrix result = pair.dm % pair.vm;
        spdlog::debug("Have {} non zero values.", result.nonZeros());
        lock.Unlock();
        return result;
    }

    MatrixPair &TemporalMatricies::get_matrix_pair(std::size_t i) {
        return matricies.at(i);
    }

}

