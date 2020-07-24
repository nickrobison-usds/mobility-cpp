//
// Created by Nicholas Robison on 7/22/20.
//

#include "map-tile/coordinates/LocaleLocator.hpp"

namespace mt::coordinates {

    LocaleLocator::LocaleLocator(const std::vector<value> &tiles) : _index(tiles.begin(), tiles.end()) {
    }

    hpx::naming::id_type LocaleLocator::get_locale(const mt::coordinates::Coordinate2D &coords) const {
        std::vector<value> values;

        // Use the index to filter down the list
        _index.query(bg::index::covers(coords), std::back_inserter(values));

        if (values.empty()) {
            throw std::invalid_argument("Out of bounds");
        }
        return values[0].second;
    }
}


