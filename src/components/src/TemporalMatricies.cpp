//
// Created by Nicholas Robison on 6/4/20.
//

#include "TemporalMatricies.hpp"
#include "spdlog/spdlog.h"

namespace components {

    std::vector<MatrixPair>
    initializeMatricies(const size_t matricies, const size_t x_dimension, const size_t y_dimension) {
        spdlog::debug("Allocating {} matricies of size {}/", matricies, x_dimension, y_dimension);
        std::vector<MatrixPair> pairs;
        pairs.reserve(matricies);
        for (int i = 0; i < matricies; i++) {
            pairs.emplace_back(
                    visit_matrix(x_dimension, y_dimension, 0),
                    distance_matrix(x_dimension, y_dimension, 0)
            );
        }

        return pairs;
    }

    TemporalMatricies::TemporalMatricies(const size_t matricies, const size_t x_dimension, const size_t y_dimension)
            : _locks(matricies),
              matricies(initializeMatricies(
                      matricies,
                      x_dimension,
                      y_dimension)) {}

    void
    TemporalMatricies::insert(std::size_t time, std::size_t x, std::size_t y, std::uint16_t visits, double distance) {
        // TODO: This is incredibly inefficient. We should take the lock before we do the inserts
        auto &lock = _locks.at(time);
        lock.Lock();
        MatrixPair &pair = matricies.at(time);
        const std::uint32_t v2 = pair.vm.at(x, y);
        const double d2 = pair.dm.at(x, y);
        pair.vm.set(x, y, visits + v2);
        pair.dm.set(x, y, distance + d2);
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
    };

}

