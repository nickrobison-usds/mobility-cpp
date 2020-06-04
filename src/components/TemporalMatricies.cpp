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
        auto &lock = _locks.at(time);
        lock.Lock();
        MatrixPair &pair = matricies.at(time);
        pair.vm(x, y) += visits;
        pair.dm(x, y) += distance;
        lock.Unlock();
    }

    void TemporalMatricies::compute() {
        for (int i = 0; i < matricies.size(); i++) {
            spdlog::debug("Computing matricies at time offset: {}", i);
            auto &lock = _locks.at(i);
            lock.Lock();
            MatrixPair &pair = matricies.at(i);
            const auto result = pair.dm * pair.vm;
            spdlog::debug("Have {} non zero values.", result.nonZeros());
            lock.Unlock();
        }
    }
}

