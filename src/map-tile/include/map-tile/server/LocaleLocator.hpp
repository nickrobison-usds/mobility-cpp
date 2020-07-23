//
// Created by Nicholas Robison on 7/22/20.
//

#ifndef MOBILITY_CPP_LOCALELOCATOR_HPP
#define MOBILITY_CPP_LOCALELOCATOR_HPP

#include "Coordinate2D.hpp"
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/index/indexable.hpp>


#include <vector>

namespace bg = boost::geometry;


namespace mt::coordinates {

    class LocaleLocator {

    public:
        typedef bg::model::box<Coordinate2D> mt_tile;
        typedef std::pair<mt_tile, std::size_t> value;

        explicit LocaleLocator(const std::vector<value> &tiles);

        [[nodiscard]] std::size_t get_locale(const Coordinate2D &coords) const;

    private:
        const bg::index::rtree<value, boost::geometry::index::linear<10>> _index;

    };
}


#endif //MOBILITY_CPP_LOCALELOCATOR_HPP
