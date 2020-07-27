//
// Created by Nicholas Robison on 7/27/20.
//

#include "map-tile/coordinates/Coordinate3D.hpp"

namespace mt::coordinates {
    Coordinate3D::Coordinate3D(const std::size_t &dim0, const std::size_t &dim1, const std::size_t &dim2): _dim0(dim0), _dim1(dim1), _dim2(dim2) {}

    std::array<std::size_t, 3> Coordinate3D::getDimensions() const {
        return {
            _dim0,
            _dim1,
            _dim2
        };
    }

    std::size_t Coordinate3D::get_dim0() const {
        return _dim0;
    }

    void Coordinate3D::set_dim0(const std::size_t &dim0) {
        _dim0 = dim0;
    }

    std::size_t Coordinate3D::get_dim1() const {
        return _dim1;
    }

    void Coordinate3D::set_dim1(const std::size_t &dim1) {
        _dim1 = dim1;
    }

    std::size_t Coordinate3D::get_dim2() const {
        return _dim2;
    }

    void Coordinate3D::set_dim2(const std::size_t &dim2) {
        _dim2 = dim2;
    }

    bool Coordinate3D::operator==(const Coordinate3D &rhs) const {
        return _dim0 == rhs.get_dim0() && _dim1 == rhs.get_dim1() && _dim2 == rhs.get_dim2();
    }


}