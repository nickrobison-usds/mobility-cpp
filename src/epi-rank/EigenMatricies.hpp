//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_EIGENMATRICIES_HPP
#define MOBILITY_CPP_EIGENMATRICIES_HPP

#include <absl/synchronization/mutex.h>
#include <Eigen/Dense>
#include <vector>
#include <utility>

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic> matrix_type;

class EigenMatricies {
public:
    EigenMatricies(size_t matricies, size_t col_dimension, size_t row_dimension);

    void insert(std::size_t time, std::size_t col, std::size_t row, double visits);

    matrix_type &get_matrix_pair(std::size_t i);

private:
    std::vector<matrix_type> _matricies;
    std::vector<absl::Mutex> _locks;

};


#endif //MOBILITY_CPP_EIGENMATRICIES_HPP
