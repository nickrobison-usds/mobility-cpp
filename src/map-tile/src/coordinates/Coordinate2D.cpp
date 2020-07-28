//
// Created by Nicholas Robison on 7/22/20.
//

#include "map-tile/coordinates/Coordinate2D.hpp"
#include <spdlog/fmt/fmt.h>

namespace mt::coordinates {

    Coordinate2D::Coordinate2D(const std::size_t &dim0, const std::size_t &dim1) : _dim0(dim0), _dim1(dim1) {
    }

    std::array<std::size_t, Coordinate2D::dimensions> Coordinate2D::getDimensions() const {
        return {
                _dim0,
                _dim1
        };
    }

    std::size_t Coordinate2D::get_dim0() const {
        return _dim0;
    }

    std::size_t Coordinate2D::get_dim1() const {
        return _dim1;
    }

    void Coordinate2D::set_dim0(const std::size_t &dim0) {
        _dim0 = dim0;
    }

    void Coordinate2D::set_dim1(const std::size_t &dim1) {
        _dim1 = dim1;
    }

    bool Coordinate2D::operator==(const Coordinate2D &rhs) const {
        return _dim0 == rhs.get_dim0() && _dim1 == rhs.get_dim1();
    }

    bool Coordinate2D::within(const bg::model::box<Coordinate2D> &rhs) const {
        return *this >= rhs.min_corner() && *this < rhs.max_corner();
    }

    bool Coordinate2D::operator<(const Coordinate2D &rhs) const {
        return _dim0 < rhs._dim0 && _dim1 < rhs._dim1;
    }

    bool Coordinate2D::operator>=(const Coordinate2D &rhs) const {
        return _dim0 >= rhs._dim0 && _dim1 >= rhs._dim1;
    }

    std::ostream &operator<<(std::ostream &os, const Coordinate2D &coords) {
        return os << fmt::format("{dim0: {}, dim1:{}}", coords.get_dim0(), coords.get_dim1());
    }
}


