//
// Created by Nicholas Robison on 7/21/20.
//

#ifndef MOBILITY_CPP_MAPSERVER_HPP
#define MOBILITY_CPP_MAPSERVER_HPP

#include "../Context.hpp"
#include <hpx/include/actions.hpp>
#include <hpx/include/components.hpp>

#include <hpx/preprocessor/cat.hpp>

namespace mt::server {

    template<typename InputKey, typename MapKey, class Mapper>
    class MapServer : public hpx::components::component_base<MapServer<InputKey, MapKey, Mapper>> {

    public:
        MapServer(MapContext <MapKey> ctx) : _ctx(ctx), _m(Mapper()) {
            // Not used
        };

        void map(const InputKey &input) const {
            _m.map(_ctx, input);
        };
        HPX_DEFINE_COMPONENT_ACTION(MapServer, map);


    private:
        const MapContext <MapKey> _ctx;
        const Mapper _m;
    };
}

#define REGISTER_MAPPER(input_key, map_key, mapper)                        \
    using HPX_PP_CAT(HPX_PP_CAT(__MapServer_map_action_, mapper), _type) = \
         ::mt::server::MapServer<input_key, map_key, mapper>::map_action;  \
    HPX_REGISTER_ACTION_DECLARATION(                                       \
        HPX_PP_CAT(HPX_PP_CAT(__MapServer_map_action_, mapper), _type),    \
        HPX_PP_CAT(__MapServer_map_action_, mapper));                      \
        \
    typedef ::hpx::components::component<::mt::server::MapServer<input_key, map_key, mapper>> HPX_PP_CAT(__MapServer, mapper); \
    HPX_REGISTER_COMPONENT(HPX_PP_CAT(__MapServer, mapper)) \


#endif //MOBILITY_CPP_MAPSERVER_HPP
