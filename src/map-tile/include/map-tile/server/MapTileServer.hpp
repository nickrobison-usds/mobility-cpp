//
// Created by Nicholas Robison on 7/19/20.
//

#ifndef MOBILITY_CPP_MAPTILESERVER_HPP
#define MOBILITY_CPP_MAPTILESERVER_HPP

#include "map-tile/client/MapTileClient.hpp"
#include "map-tile/ctx/Context.hpp"
#include "map-tile/io/FileProvider.hpp"
#include "traits.hpp"
#include <hpx/include/actions.hpp>
#include <hpx/include/components.hpp>
#include <hpx/preprocessor/cat.hpp>
#include <hpx/serialization/map.hpp>
#include <hpx/include/parallel_for_each.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>
#include <functional>
#include <map>
#include <utility>
#include <vector>

using namespace std;

namespace mt::server {


    void tile() {
        const auto locales = hpx::find_all_localities();
        // For each locality, create a tile
    }


    template<
            class MapKey,
            class Coordinate,
            class Mapper,
            class Tiler,
            class InputKey = std::string,
            template<typename = InputKey> class Provider = io::FileProvider
    >
    class MapTileServer
            : public hpx::components::component_base<MapTileServer<MapKey, Coordinate, Mapper, Tiler, InputKey, Provider>> {
    public:
        typedef typename coordinates::LocaleLocator<Coordinate>::mt_tile mt_tile;

        explicit MapTileServer(const coordinates::LocaleLocator<Coordinate> &locator,
                               const mt_tile &tile,
                               const std::map<string, string> &config,
                               vector<string> files) : _files(
                std::move(files)), _locator(locator), _tiler(Tiler{}),
                                                       _ctx(std::bind(&MapTileServer::handle_emit, this,
                                                                      std::placeholders::_1,
                                                                      std::placeholders::_2),
                                                            tile, config) {
            // Not used
        }

        void initialize() {
            // Setup the tiler
            if constexpr (has_setup<Tiler, MapKey, Coordinate>::value) {
                _tiler.setup(_ctx);
            }
        }

        HPX_DEFINE_COMPONENT_ACTION(MapTileServer, initialize);

        void tile() {
            // Load the CSV files
            // Let's do it all in memory for right now
            const auto ctx = _ctx;
            Mapper mapper;
            // Call the setup method, if one exists
            // TODO: This will need to be further improved. Right now it will fail if a setup() method exists with the wrong arguments.
            if constexpr(has_setup<Mapper, MapKey, Coordinate>::value) {
                mapper.setup(ctx);
            }
            for_each(_files.begin(), _files.end(), [&ctx, &mapper](const string &filename) {
                spdlog::debug("Reading {}", filename);
                Provider<InputKey> provider(filename);
                vector<InputKey> keys = provider.provide();
                // Map each one

                //Map
                hpx::parallel::for_each(hpx::parallel::execution::par, keys.begin(), keys.end(),
                                        [&ctx, &mapper](const auto &key) {
                                            mapper.map(ctx, key);
                                        });
            });
        }

        HPX_DEFINE_COMPONENT_ACTION(MapTileServer, tile);

        void receive(const Coordinate &key, const MapKey &value) {
            _tiler.receive(_ctx, key, value);
        }

        HPX_DEFINE_COMPONENT_ACTION(MapTileServer, receive);

        void compute() {
            _tiler.compute(_ctx);
        }

        HPX_DEFINE_COMPONENT_ACTION(MapTileServer, compute);

    private:
        const vector<string> _files;
        const coordinates::LocaleLocator<Coordinate> _locator;
        const ctx::Context<MapKey, Coordinate> _ctx;
        Tiler _tiler;

        void handle_emit(const Coordinate &key, const MapKey &value) const {
            // We do this manually to avoid pull in the MapClient header
            const auto locale_num = _locator.get_locale(key);
            const auto id = hpx::find_from_basename("/mt/base", locale_num).get();

            typedef typename mt::server::MapTileServer<MapKey, Coordinate, Mapper, Tiler>::receive_action action_type;
            try {
                hpx::async<action_type>(id, key, value).get();
            } catch (const std::exception &e) {
                spdlog::debug("Unable to send value. {}", e.what());
            }
        }
    };
}

#define REGISTER_MAPPER(map_key, coordinate, mapper, tiler, input_key, provider)                        \
    using HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_tile_action_, mapper), _type) = \
         ::mt::server::MapTileServer<map_key, coordinate, mapper, tiler, input_key, provider>::tile_action;  \
    HPX_REGISTER_ACTION(                                       \
        HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_tile_action_, mapper), _type),    \
        HPX_PP_CAT(__MapTileServer_tile_action_, mapper));                      \
        \
    using HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_receive_action_, mapper), _type) = \
         ::mt::server::MapTileServer<map_key, coordinate, mapper, tiler, input_key, provider>::receive_action;  \
    HPX_REGISTER_ACTION(                                       \
        HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_receive_action_, mapper), _type),    \
        HPX_PP_CAT(__MapTileServer_receive_action_, mapper));                                        \
                                                                                                        \
    using HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_compute_action_, mapper), _type) = \
         ::mt::server::MapTileServer<map_key, coordinate, mapper, tiler, input_key, provider>::compute_action;  \
    HPX_REGISTER_ACTION(                                       \
        HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_compute_action_, mapper), _type),    \
        HPX_PP_CAT(__MapTileServer_compute_action_, mapper));                                           \
                                                                                                        \
    using HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_initialize_action_, mapper), _type) = \
         ::mt::server::MapTileServer<map_key, coordinate, mapper, tiler, input_key, provider>::initialize_action;  \
    HPX_REGISTER_ACTION(                                       \
        HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_initialize_action_, mapper), _type),    \
        HPX_PP_CAT(__MapTileServer_initialize_action_, mapper));                                        \
                                                                                                        \
    typedef ::hpx::components::component<::mt::server::MapTileServer<map_key, coordinate, mapper, tiler, input_key, provider>> HPX_PP_CAT(__MapTileServer, mapper); \
    HPX_REGISTER_COMPONENT(HPX_PP_CAT(__MapTileServer, mapper)) \

#endif //MOBILITY_CPP_MAPTILESERVER_HPP
