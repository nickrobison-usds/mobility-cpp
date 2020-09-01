//
// Created by Nicholas Robison on 7/22/20.
//

#ifndef MOBILITY_CPP_LOCALELOCATOR_HPP
#define MOBILITY_CPP_LOCALELOCATOR_HPP

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

    template<typename Coordinate>
    class LocaleLocator {

    public:
        typedef bg::model::box<Coordinate> mt_tile;
        typedef std::pair<mt_tile, std::uint64_t> value;

        explicit LocaleLocator(const std::vector<value> &tiles) : _index(tiles.begin(), tiles.end()) {
            // Not used
            const auto sz = tiles.size();
            if (sz == 0) {
                throw std::invalid_argument("Nothing here");
            }
        };

        LocaleLocator<Coordinate>() = default;

        [[nodiscard]] std::size_t get_num_locales() const {
            return _index.size();
        }

        [[nodiscard]] std::uint64_t get_locale(const Coordinate &coords) const {
            std::vector<value> values;

            // Use the index to filter down the list
            _index.query(bg::index::covers(coords), std::back_inserter(values));
            if (values.empty()) {
                throw std::invalid_argument("Out of bounds");
            }

            const auto iter = std::find_if(values.begin(), values.end(), [&coords](const auto &box) {
                // TODO: This should be SIMDified.
               const auto within = coords.within(box.first);
               return within;
            });

            if (iter == std::end(values)) {
                throw std::invalid_argument("Out of bounds");
            }

            return iter->second;
        }

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
