//
// Created by Nicholas Robison on 6/10/20.
//

#ifndef MOBILITY_CPP_OFFSETCALCULATOR_HPP
#define MOBILITY_CPP_OFFSETCALCULATOR_HPP

#include "server/ShapefileServer.hpp"
#include "shared/TileConfiguration.hpp"
#include <boost/bimap.hpp>
#include <optional>

namespace components::detail {

    typedef boost::bimap<std::string, std::size_t> offset_bimap;
    typedef offset_bimap::value_type position;

    class OffsetCalculator {
    public:
        explicit OffsetCalculator(const server::ShapefileServer::offset_type &init, const TileConfiguration &config);

        /**
     * Compute the global offset for a given CBG code
     * @param cbg_code - CBG code (string)
     * @return offset
     */
        [[nodiscard]] std::optional<std::size_t> calculate_cbg_offset(const std::string &cbg_code) const;

        [[nodiscard]] std::size_t calculate_local_offset(const std::string &cbg_code) const;

        [[nodiscard]] std::optional<std::string> cbg_from_local_offset(size_t cbg_idx) const;

        [[nodiscard]] optional<string> cbg_from_offset(size_t cbg_idx) const;

    private:
        const std::size_t _start_idx;
        const std::size_t _end_idx;
        const offset_bimap _cbg_map;
    };
}


#endif //MOBILITY_CPP_OFFSETCALCULATOR_HPP
