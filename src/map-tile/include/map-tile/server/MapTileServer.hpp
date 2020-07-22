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

#include "map-tile/Context.hpp"
#include "../../../src/FileProvider.hpp"
#include "MapServer.hpp"

using namespace std;

namespace mt::server {
    template<
            class MapKey,
            class ReduceKey,
            class Mapper,
            class InputKey = std::string,
            template<typename = InputKey> class Provider = io::FileProvider
    >
    class MapTileServer
            : public hpx::components::component_base<MapTileServer<MapKey, ReduceKey, Mapper, InputKey, Provider>> {
    public:
        explicit MapTileServer(vector<string> files) : _files(std::move(files)) {
            // Not used
        }

        void tile() {
            // Instantiate the context
            const auto ctx = make_shared<mt::Context<MapKey>>();
            // Load the CSV files
            // Let's do it all in memory for right now
            for_each(_files.begin(), _files.end(), [&ctx](const string &filename) {
                Provider<InputKey> provider(filename);
                vector<InputKey> keys = provider.provide();
                // Map each one

                Mapper mapper;
                // Setup

                //Map
                for_each(keys.begin(), keys.end(), [&ctx, &mapper](const auto &key) {
                    mapper.map(*ctx, key);;
                });
            });

            // Now, tile
        }

        HPX_DEFINE_COMPONENT_ACTION(MapTileServer, tile);

    private:
        const vector<string> _files;

    };
}

#define REGISTER_MAPPER(map_key, reduce_key, mapper, input_key, provider)                        \
    using HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_tile_action_, mapper), _type) = \
         ::mt::server::MapTileServer<map_key, reduce_key, mapper, input_key, provider>::tile_action;  \
    HPX_REGISTER_ACTION(                                       \
        HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_tile_action_, mapper), _type),    \
        HPX_PP_CAT(__MapTileServer_tile_action_, mapper));                      \
        \
    typedef ::hpx::components::component<::mt::server::MapTileServer<map_key, reduce_key, mapper, input_key, provider>> HPX_PP_CAT(__MapTileServer, mapper); \
    HPX_REGISTER_COMPONENT(HPX_PP_CAT(__MapTileServer, mapper)) \

#endif //MOBILITY_CPP_MAPTILESERVER_HPP
