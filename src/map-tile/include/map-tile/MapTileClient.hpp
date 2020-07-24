//
// Created by Nicholas Robison on 7/22/20.
//

#ifndef MOBILITY_CPP_MAPTILECLIENT_HPP
#define MOBILITY_CPP_MAPTILECLIENT_HPP

#include "server/MapTileServer.hpp"
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>

namespace mt {

    template<
            class MapKey,
            class ReduceKey,
            class Mapper
    >
    class MapTileClient
            : public hpx::components::client_base<
                    MapTileClient<MapKey, ReduceKey, Mapper>,
                    ::mt::server::MapTileServer<MapKey, ReduceKey, Mapper>> {

        typedef typename ::mt::server::MapTileServer<MapKey, ReduceKey, Mapper> MTS;
        typedef hpx::components::client_base<
                MapTileClient<MapKey, ReduceKey, Mapper>,
                MTS> base_type;

    public:


        explicit MapTileClient(hpx::future<hpx::id_type> &&f) : base_type(std::move(f)) {};

        explicit MapTileClient(hpx::id_type &&f) : base_type(std::move(f)) {};

        explicit MapTileClient(const coordinates::LocaleLocator &locator, std::vector<std::string> files) : base_type(hpx::new_<MTS>(hpx::find_here(), locator, files)) {
            // Not used
        }

        void tile() {
            typedef typename ::mt::server::MapTileServer<MapKey, ReduceKey, Mapper>::tile_action action_type;
            return action_type()(this->get_id());
        }

        void receive(const coordinates::Coordinate2D key, const MapKey value) {
            typedef typename ::mt::server::MapTileServer<MapKey, ReduceKey, Mapper>::receive_action action_type;
            return action_type()(this->get_id(), key, value);
        }
    };
}

#endif //MOBILITY_CPP_MAPTILECLIENT_HPP
