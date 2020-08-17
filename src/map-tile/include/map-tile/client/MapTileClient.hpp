//
// Created by Nicholas Robison on 7/22/20.
//

#ifndef MOBILITY_CPP_MAPTILECLIENT_HPP
#define MOBILITY_CPP_MAPTILECLIENT_HPP

#include "map-tile/server/MapTileServer.hpp"
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>
#include <spdlog/fmt/fmt.h>

namespace mt::client {

    template<
            class MapKey,
            class Coordinate,
            class Mapper,
            class Tiler
    >
    class MapTileClient
            : public hpx::components::client_base<
                    MapTileClient<MapKey, Coordinate, Mapper, Tiler>,
                    mt::server::MapTileServer<MapKey, Coordinate, Mapper, Tiler>> {

        typedef typename mt::server::MapTileServer<MapKey, Coordinate, Mapper, Tiler> MTS;
        typedef hpx::components::client_base<
                MapTileClient<MapKey, Coordinate, Mapper, Tiler>,
                MTS> base_type;

    public:
        typedef typename coordinates::LocaleLocator<Coordinate>::mt_tile mt_tile;
        typedef typename std::pair<mt_tile, std::uint64_t> tile_pair;

        explicit MapTileClient(hpx::future<hpx::id_type> &&f) : base_type(std::move(f)) {};

        explicit MapTileClient(hpx::id_type &&f) : base_type(std::move(f)) {};

        explicit MapTileClient(const hpx::id_type &id,
                               const coordinates::LocaleLocator<Coordinate> &locator,
                               const tile_pair &tile,
                               const std::map<string, string> &config,
                               std::vector<std::string> files) : base_type(
                hpx::new_<MTS>(id, locator, tile.first, config, files)) {
            hpx::register_with_basename(fmt::format("mt/base/{}", tile.second), this->get_id());
        }

        hpx::future<void> tile() {
            typedef typename mt::server::MapTileServer<MapKey, Coordinate, Mapper, Tiler>::tile_action action_type;
            return hpx::async<action_type>(this->get_id());
        }

        hpx::future<void> compute() {
            typedef typename mt::server::MapTileServer<MapKey, Coordinate, Mapper, Tiler>::compute_action action_type;
            return hpx::async<action_type>(this->get_id());
        }

        hpx::future<void> initialize() {
            typedef typename mt::server::MapTileServer<MapKey, Coordinate, Mapper, Tiler>::initialize_action action_type;
            return hpx::async<action_type>(this->get_id());
        }

        hpx::future<double> reduce() {
            typedef typename mt::server::MapTileServer<MapKey, Coordinate, Mapper, Tiler>::reduce_action action_type;
            return hpx::async<action_type>(this->get_id());
        }

        void receive(hpx::launch::apply_policy, const Coordinate key, const MapKey value) {
            typedef typename mt::server::MapTileServer<MapKey, Coordinate, Mapper, Tiler>::receive_action action_type;
            return hpx::apply<action_type>(this->get_id(), key, value);
        }
    };
}

#endif //MOBILITY_CPP_MAPTILECLIENT_HPP
