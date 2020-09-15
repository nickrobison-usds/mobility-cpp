//
// Created by Nicholas Robison on 9/15/20.
//

#ifndef MOBILITY_CPP_BLAZEMATRICIES_HPP
#define MOBILITY_CPP_BLAZEMATRICIES_HPP

#include <utility>
#include <absl/synchronization/mutex.h>
#include <blaze/math/DynamicMatrix.h>


class BlazeMatricies {
public:
    typedef blaze::DynamicMatrix<double, blaze::columnMajor> matrix_type;
    BlazeMatricies(size_t matricies, size_t col_dimension, size_t row_dimension);

    void insert(std::size_t time, std::size_t col, std::size_t row, double visits);

    BlazeMatricies::matrix_type &get_matrix_pair(std::size_t i);

private:
    std::vector<BlazeMatricies::matrix_type> matricies;
    std::vector<absl::Mutex> _locks;
};


#endif //MOBILITY_CPP_BLAZEMATRICIES_HPP
