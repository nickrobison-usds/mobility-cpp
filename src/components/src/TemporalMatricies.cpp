//
// Created by Nicholas Robison on 6/4/20.
//

#include "TemporalMatricies.hpp"
#include "spdlog/spdlog.h"

namespace components {

    std::vector<MatrixPair> initializeMatricies(const size_t matricies, const size_t dimensions) {
        spdlog::debug("Allocating {} matricies of size {}", matricies, dimensions);
        std::vector<MatrixPair> pairs;
        pairs.reserve(matricies);
        for (int i = 0; i < matricies; i++) {
            pairs.emplace_back(
                    visit_matrix(dimensions, dimensions, 0),
                    distance_matrix(dimensions, dimensions, 0)
            );
        }

        return pairs;
    }

    TemporalMatricies::TemporalMatricies(const size_t matricies, const size_t dimensions) : _locks(matricies),
                                                                                            matricies(
                                                                                                    initializeMatricies(
                                                                                                            matricies,
                                                                                                            dimensions)) {}

    void
    TemporalMatricies::insert(std::size_t time, std::size_t x, std::size_t y, std::uint16_t visits, double distance) {
        // TODO: This is incredibly inefficient. We should take the lock before we do the inserts
        auto &lock = _locks.at(time);
        lock.Lock();
        MatrixPair &pair = matricies.at(time);
        const auto v2 = pair.vm.at(x, y);
        const auto d2 = pair.dm.at(x, y);
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
    };
}

