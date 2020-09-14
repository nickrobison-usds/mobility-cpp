//
// Created by Nicholas Robison on 6/10/20.
//

#ifndef MOBILITY_CPP_CBGOFFSETCALCULATOR_HPP
#define MOBILITY_CPP_CBGOFFSETCALCULATOR_HPP

#include "components/OffsetCalculator.hpp"
#include "components/server/ShapefileServer.hpp"
#include "shared/TileConfiguration.hpp"
#include "offset_shared.hpp"
#include <optional>

namespace components::detail {



    class CBGOffsetCalculator : public OffsetCalculator<CBGOffsetCalculator> {
    public:
        explicit CBGOffsetCalculator(const server::ShapefileServer::offset_type &init, const TileConfiguration &config);

        /**
     * Compute the global offset for a given CBG code
     * @param cbg_code - CBG code (string)
     * @return offset
     */
        [[nodiscard]] std::optional<std::size_t> to_global_offset_impl(const std::string_view cbg_code) const;

        [[nodiscard]] std::size_t to_local_offset_impl(const std::string_view cbg_code) const;

        [[nodiscard]] std::optional<std::string> from_local_offset_impl(size_t cbg_idx) const;

        [[nodiscard]] optional<string> from_global_offset_impl(size_t cbg_idx) const;

    private:
        const std::size_t _start_idx;
        const std::size_t _end_idx;
        const offset_bimap _cbg_map;
    };
}


#endif //MOBILITY_CPP_CBGOFFSETCALCULATOR_HPP
