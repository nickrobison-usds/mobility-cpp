//
// Created by Nicholas Robison on 7/19/20.
//

#ifndef MOBILITY_CPP_MAPTILESERVER_HPP
#define MOBILITY_CPP_MAPTILESERVER_HPP

#include <algorithm>
#include <utility>
#include <vector>
#include <io/csv_reader.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/components.hpp>
#include <hpx/preprocessor/cat.hpp>

#include "map-tile/client/MapTileClient.hpp"
#include "map-tile/ctx/Context.hpp"
#include "../../../src/FileProvider.hpp"

#include <functional>

using namespace std;

namespace mt::server {


    void tile() {
        const auto locales = hpx::find_all_localities();
        // For each locality, create a tile
    }


    template<
            class MapKey,
            class ReduceKey,
            class Mapper,
            class Tiler,
            class InputKey = std::string,
            template<typename = InputKey> class Provider = io::FileProvider
    >
    class MapTileServer
            : public hpx::components::component_base<MapTileServer<MapKey, ReduceKey, Mapper, Tiler, InputKey, Provider>> {
    public:
        explicit MapTileServer(const coordinates::LocaleLocator &locator, vector<string> files) : _files(
                std::move(files)), _locator(locator), _tiler(Tiler{}), _ctx(std::bind(&MapTileServer::handle_emit, this,
                                                                                      std::placeholders::_1,
                                                                                      std::placeholders::_2)) {
            // Not used
        }

        void tile() {
            // Instantiate the context
            // Load the CSV files
            // Let's do it all in memory for right now
            const auto ctx = _ctx;
            for_each(_files.begin(), _files.end(), [&ctx](const string &filename) {
                Provider<InputKey> provider(filename);
                vector<InputKey> keys = provider.provide();
                // Map each one

                Mapper mapper;
                // This should be a const expr check
                mapper.setup();
                // Setup

                //Map
                for_each(keys.begin(), keys.end(), [&ctx, &mapper](const auto &key) {
                    mapper.map(ctx, key);;
                });
            });

            _tiler.compute();
        }

        HPX_DEFINE_COMPONENT_ACTION(MapTileServer, tile);

        void receive(const coordinates::Coordinate2D &key, const MapKey &value) {
            _tiler.receive(_ctx, key, value);
        }

        HPX_DEFINE_COMPONENT_ACTION(MapTileServer, receive);

    private:
        const vector<string> _files;
        const coordinates::LocaleLocator _locator;
        const ctx::Context<MapKey> _ctx;
        Tiler _tiler;

        void handle_emit(const coordinates::Coordinate2D &key, const MapKey &value) const {
            // We do this manually to avoid pull in the MapClient header
            const auto locale_num = _locator.get_locale(key);
            const auto id = hpx::find_from_basename("/mt/base", locale_num).get();

            typedef typename mt::server::MapTileServer<MapKey, ReduceKey, Mapper, Tiler>::receive_action action_type;
            try {
                std::cout << "Sending" << std::endl;
                hpx::async<action_type>(id, key, value).get();
            } catch (const std::exception &e) {
                std::cout << "Wrong" << std::endl;
                e.what();
            }
        }
    };
}

#define REGISTER_MAPPER(map_key, reduce_key, mapper, tiler, input_key, provider)                        \
    using HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_tile_action_, mapper), _type) = \
         ::mt::server::MapTileServer<map_key, reduce_key, mapper, tiler, input_key, provider>::tile_action;  \
    HPX_REGISTER_ACTION(                                       \
        HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_tile_action_, mapper), _type),    \
        HPX_PP_CAT(__MapTileServer_tile_action_, mapper));                      \
        \
    using HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_receive_action_, mapper), _type) = \
         ::mt::server::MapTileServer<map_key, reduce_key, mapper, tiler, input_key, provider>::receive_action;  \
    HPX_REGISTER_ACTION(                                       \
        HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_receive_action_, mapper), _type),    \
        HPX_PP_CAT(__MapTileServer_receive_action_, mapper));                      \
        \
    typedef ::hpx::components::component<::mt::server::MapTileServer<map_key, reduce_key, mapper, tiler, input_key, provider>> HPX_PP_CAT(__MapTileServer, mapper); \
    HPX_REGISTER_COMPONENT(HPX_PP_CAT(__MapTileServer, mapper)) \

#endif //MOBILITY_CPP_MAPTILESERVER_HPP
