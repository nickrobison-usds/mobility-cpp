//
// Created by Nicholas Robison on 7/21/20.
//

#ifndef MOBILITY_CPP_MAPSERVERCLIENT_HPP
#define MOBILITY_CPP_MAPSERVERCLIENT_HPP

#include "server/MapServer.hpp"
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>

namespace mt {

    template<class InputKey, class MapKey, template<typename = InputKey, typename = MapKey> class Mapper>
    class MapServerClient
            : public hpx::components::client_base<MapServerClient<InputKey, MapKey, Mapper>, mt::server::MapServer<InputKey, MapKey,
                    Mapper<InputKey, Mapper>>> {

    public:
        explicit MapServerClient(const MapContext<MapKey & ctx) : hpx::new_<server::MapServer<InputKey, MapKey, Mapper>>(hpx::find_here(), ctx) {
            // Not used
        }

        hpx::future<void> map(const InputKey &key) const {
            return hpx::async<server::MapServer::map_action(get_id(), key);
        }
    }

    }

#endif //MOBILITY_CPP_MAPSERVERCLIENT_HPP
