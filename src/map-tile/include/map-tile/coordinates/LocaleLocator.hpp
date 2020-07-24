//
// Created by Nicholas Robison on 7/22/20.
//

#ifndef MOBILITY_CPP_LOCALELOCATOR_HPP
#define MOBILITY_CPP_LOCALELOCATOR_HPP

#include "Coordinate2D.hpp"
#include <hpx/include/naming.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/index/indexable.hpp>


#include <vector>

namespace bg = boost::geometry;


namespace mt::coordinates {

    typedef bg::model::box<Coordinate2D> mt_tile;

    class LocaleLocator {

    public:
        typedef std::pair<mt_tile, hpx::naming::id_type> value;

        explicit LocaleLocator(const std::vector<value> &tiles);
        LocaleLocator() = default;

        [[nodiscard]] hpx::naming::id_type get_locale(const Coordinate2D &coords) const;

    private:
        const bg::index::rtree<value, boost::geometry::index::linear<10>> _index;

        // HPX required serialization
        // TODO: This actually needs to be serializable
        friend class hpx::serialization::access;
        template<typename Archive>
        void serialize(Archive &ar, const unsigned int version) {
        }

    };
}


#endif //MOBILITY_CPP_LOCALELOCATOR_HPP
