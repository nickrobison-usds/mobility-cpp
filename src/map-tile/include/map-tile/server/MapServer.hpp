//
// Created by Nicholas Robison on 7/21/20.
//

#ifndef MOBILITY_CPP_MAPSERVER_HPP
#define MOBILITY_CPP_MAPSERVER_HPP

#include "../ctx/Context.hpp"
#include <hpx/include/actions.hpp>
#include <hpx/include/components.hpp>

namespace mt::server {

    template<typename InputKey, typename MapKey, class Mapper>
    class MapServer : public hpx::components::component_base<MapServer<InputKey, MapKey, Mapper>> {

    public:
        MapServer(ctx::MapContext <MapKey> ctx) : _ctx(ctx), _m(Mapper()) {
            // Not used
        };

        void map(const InputKey &input) const {
            _m.map(_ctx, input);
        };
        HPX_DEFINE_COMPONENT_ACTION(MapServer, map);


    private:
        const ctx::MapContext <MapKey> _ctx;
        const Mapper _m;
    };
}

#endif //MOBILITY_CPP_MAPSERVER_HPP
