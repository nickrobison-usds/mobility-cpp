//
// Created by Nicholas Robison on 6/4/20.
//

#ifndef MOBILITY_CPP_TEMPORALMATRICIES_HPP
#define MOBILITY_CPP_TEMPORALMATRICIES_HPP

#include <utility>
#include <absl/synchronization/mutex.h>
#include <blaze/math/CompressedMatrix.h>

typedef blaze::CompressedMatrix<uint32_t, blaze::columnMajor> visit_matrix;
typedef blaze::CompressedMatrix<double, blaze::columnMajor> distance_matrix;

namespace components {

    struct MatrixPair {
        visit_matrix vm;
        distance_matrix dm;

        MatrixPair(visit_matrix vm, distance_matrix dm) : vm(std::move(vm)),
                                                          dm(std::move(dm)) {}
    };


    class TemporalMatricies {

    public:
        TemporalMatricies(size_t matricies, size_t col_dimension, size_t row_dimension);

        void insert(std::size_t time, std::size_t col, std::size_t row, std::uint16_t visits, double distance);

        distance_matrix compute(std::size_t i = 0);

        MatrixPair &get_matrix_pair(std::size_t i);

    private:
        std::vector<MatrixPair> matricies;
        std::vector<absl::Mutex> _locks;
    };
}


#endif //MOBILITY_CPP_TEMPORALMATRICIES_HPP
