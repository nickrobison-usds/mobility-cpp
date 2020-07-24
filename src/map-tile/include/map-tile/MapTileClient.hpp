//
// Created by Nicholas Robison on 7/22/20.
//

#ifndef MOBILITY_CPP_MAPTILECLIENT_HPP
#define MOBILITY_CPP_MAPTILECLIENT_HPP

#include "server/MapTileServer.hpp"
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>

namespace mt::client {

    template<
            class MapKey,
            class ReduceKey,
            class Mapper,
            class Tiler
    >
    class MapTileClient
            : public hpx::components::client_base<
                    MapTileClient<MapKey, ReduceKey, Mapper, Tiler>,
                    mt::server::MapTileServer<MapKey, ReduceKey, Mapper, Tiler>> {

        typedef typename mt::server::MapTileServer<MapKey, ReduceKey, Mapper, Tiler> MTS;
        typedef hpx::components::client_base<
                MapTileClient<MapKey, ReduceKey, Mapper, Tiler>,
                MTS> base_type;

    public:


        explicit MapTileClient(hpx::future<hpx::id_type> &&f) : base_type(std::move(f)) {};

        explicit MapTileClient(hpx::id_type &&f) : base_type(std::move(f)) {};

        explicit MapTileClient(const coordinates::LocaleLocator &locator, std::vector<std::string> files) : base_type(
                hpx::new_<MTS>(hpx::find_here(), locator, files)) {
            // Not used
        }

        void tile() {
            typedef typename mt::server::MapTileServer<MapKey, ReduceKey, Mapper, Tiler>::tile_action action_type;
            return action_type()(this->get_id());
        }

        void receive(hpx::launch::apply_policy, const coordinates::Coordinate2D key, const MapKey value) {
            typedef typename mt::server::MapTileServer<MapKey, ReduceKey, Mapper, Tiler>::receive_action action_type;
            return hpx::apply<action_type>(this->get_id(), key, value);
        }
    };
}

#endif //MOBILITY_CPP_MAPTILECLIENT_HPP
