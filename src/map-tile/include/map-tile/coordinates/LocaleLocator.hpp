//
// Created by Nicholas Robison on 7/22/20.
//

#ifndef MOBILITY_CPP_LOCALELOCATOR_HPP
#define MOBILITY_CPP_LOCALELOCATOR_HPP

#include "Coordinate2D.hpp"
#include <hpx/include/naming.hpp>
#include <hpx/serialization/vector.hpp>
#include <hpx/serialization/std_tuple.hpp>
#include <boost/geometry/geometries/geometries.hpp>
#include <boost/geometry/index/rtree.hpp>
#include <boost/geometry/index/indexable.hpp>
#include <algorithm>


#include <vector>

namespace bg = boost::geometry;


namespace mt::coordinates {

    typedef bg::model::box<Coordinate2D> mt_tile;

    class LocaleLocator {

    public:
        typedef std::pair<mt_tile, std::uint64_t> value;

        explicit LocaleLocator(const std::vector<value> &tiles);

        LocaleLocator() = default;

        [[nodiscard]] std::uint64_t get_locale(const Coordinate2D &coords) const;

        // HPX required serialization
        friend class hpx::serialization::access;

        template<typename Archive>
        void save(Archive &ar, const unsigned int version) const {
            std::vector<value> tmp;
            std::copy(_index.begin(), _index.end(), std::back_inserter(tmp));
            ar & tmp;
        }

        template<typename Archive>
        void load(Archive &ar, const unsigned int version) {
            std::vector<value> tmp;
            ar & tmp;

            _index.insert(tmp.begin(), tmp.end());
        }

        HPX_SERIALIZATION_SPLIT_MEMBER()

    private:
        bg::index::rtree<value, boost::geometry::index::linear<10>> _index;

    };
}

namespace hpx::serialization {

    template<typename Archive, typename Point>
    void serialize(Archive &ar, boost::geometry::model::box<Point> &box, unsigned int const) {
        ar & box.min_corner() & box.max_corner();
    }
}


#endif //MOBILITY_CPP_LOCALELOCATOR_HPP
