//
// Created by Nicholas Robison on 7/22/20.
//

#include "map-tile/coordinates/Coordinate2D.hpp"

namespace mt::coordinates {

    Coordinate2D::Coordinate2D(const std::size_t &dim0, const std::size_t &dim1) : _dims({dim0, dim1}) {
    }

    std::array<std::size_t, dimensions> Coordinate2D::getDimensions() const {
        return {
                this->template get<0>(),
                this->template get<1>()
        };
    }

    std::size_t Coordinate2D::get_dim0() const {
        return _dims[0];
    }

    std::size_t Coordinate2D::get_dim1() const {
        return _dims[1];
    }

    void Coordinate2D::set_dim0(const std::size_t &dim0) {
        _dims[0] = dim0;
    }

    void Coordinate2D::set_dim1(const std::size_t &dim1) {
        _dims[1] = dim1;
    }
}


