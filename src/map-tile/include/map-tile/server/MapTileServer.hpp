//
// Created by Nicholas Robison on 7/19/20.
//

#ifndef MOBILITY_CPP_MAPTILESERVER_HPP
#define MOBILITY_CPP_MAPTILESERVER_HPP

#include "map-tile/client/MapTileClient.hpp"
#include "map-tile/ctx/Context.hpp"
#include "map-tile/io/EmitHandler.hpp"
#include "map-tile/io/FileProvider.hpp"
#include "traits.hpp"
#include <hpx/include/actions.hpp>
#include <hpx/include/components.hpp>
#include <hpx/preprocessor/cat.hpp>
#include <hpx/serialization/map.hpp>
#include <hpx/include/parallel_for_each.hpp>
#include <shared/HostnameLogger.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>
#include <spdlog/pattern_formatter.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <algorithm>
#include <functional>
#include <map>
#include <string_view>
#include <utility>
#include <vector>

using namespace std;

namespace mt::server {


    template<
            class MapKey,
            class Coordinate,
            class Mapper,
            class Tiler,
            class ReduceValue,
            class InputKey = std::string,
            template<typename = InputKey> class Provider = io::FileProvider
    >
    class MapTileServer
            : public hpx::components::component_base<MapTileServer<MapKey, Coordinate, Mapper, Tiler, ReduceValue, InputKey, Provider>> {
    public:
        typedef typename coordinates::LocaleLocator<Coordinate>::mt_tile mt_tile;

        explicit MapTileServer(const coordinates::LocaleLocator<Coordinate> &locator,
                               const mt_tile &tile,
                               const std::map<string, string> &config,
                               vector<string> files) : _files(
                std::move(files)), _tiler(Tiler{}),
                                                       _ctx(std::bind(&MapTileServer::handle_emit, this,
                                                                      std::placeholders::_1,
                                                                      std::placeholders::_2),
                                                            tile, config),
                                                       _emitter(locator) {
            auto formatter = std::make_unique<spdlog::pattern_formatter>();
            formatter->add_flag<shared::HostnameLogger>('h').set_pattern("[%l] [%h] [%H:%M:%S %z] [thread %t] %v");
            auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();

            console_sink->set_level(spdlog::level::info);
            spdlog::logger logger("MapTileServer", {console_sink});
            logger.set_formatter(std::move(formatter));
            spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));
            spdlog::set_level(spdlog::level::debug);
        }

        void initialize() {
            // Setup the tiler
            spdlog::info("Initializing tilers");
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
            for_each(_files.begin(), _files.end(), [&ctx, &mapper, this](const string &filename) {
                spdlog::info("Reading {}", filename);
                Provider<InputKey> provider(filename);
                vector<InputKey> keys = provider.provide();
                // Map each one

                //Map
                hpx::parallel::for_each(hpx::parallel::execution::par, keys.begin(), keys.end(),
                                        [&ctx, &mapper](const auto &key) {
                                            mapper.map(ctx, key);
                                        });
            });

            // Flush any remaining values
            _emitter.flush();
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

        ReduceValue reduce() {
            // TODO: This method really needs to be conditionally enabled, but that will require changes to how we handle action registration.
            if constexpr (has_reduce<Tiler, MapKey, Coordinate, ReduceValue>::value) {
                return _tiler.reduce(_ctx);
            } else {
                throw std::logic_error("Cannot call reduce on Tiler without reduce method");
            }

        }

        HPX_DEFINE_COMPONENT_ACTION(MapTileServer, reduce);

    private:
        const vector<string> _files;
        const ctx::Context<MapKey, Coordinate> _ctx;
        Tiler _tiler;
        io::EmitHandler<mt::server::MapTileServer<MapKey, Coordinate, Mapper, Tiler, ReduceValue>, Coordinate, MapKey> _emitter;

        void handle_emit(const Coordinate &key, const MapKey &value) {
            std::pair<const Coordinate, const MapKey> pair = std::make_pair(key, value);
            _emitter.emit(std::make_shared<std::pair<const Coordinate, const MapKey>>(pair));
        }
    };
}

#define REGISTER_MAPPER(map_key, coordinate, mapper, tiler, reduce_value, input_key, provider)                        \
    using HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_tile_action_, mapper), _type) = \
         ::mt::server::MapTileServer<map_key, coordinate, mapper, tiler, reduce_value, input_key, provider>::tile_action;  \
    HPX_REGISTER_ACTION(                                       \
        HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_tile_action_, mapper), _type),    \
        HPX_PP_CAT(__MapTileServer_tile_action_, mapper));                      \
        \
    using HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_receive_action_, mapper), _type) = \
         ::mt::server::MapTileServer<map_key, coordinate, mapper, tiler, reduce_value, input_key, provider>::receive_action;  \
    HPX_REGISTER_ACTION(                                       \
        HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_receive_action_, mapper), _type),    \
        HPX_PP_CAT(__MapTileServer_receive_action_, mapper));                                        \
                                                                                                        \
    using HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_compute_action_, mapper), _type) = \
         ::mt::server::MapTileServer<map_key, coordinate, mapper, tiler, reduce_value, input_key, provider>::compute_action;  \
    HPX_REGISTER_ACTION(                                       \
        HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_compute_action_, mapper), _type),    \
        HPX_PP_CAT(__MapTileServer_compute_action_, mapper));                                           \
                                                                                                        \
    using HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_initialize_action_, mapper), _type) = \
         ::mt::server::MapTileServer<map_key, coordinate, mapper, tiler, reduce_value, input_key, provider>::initialize_action;  \
    HPX_REGISTER_ACTION(                                       \
        HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_initialize_action_, mapper), _type),    \
        HPX_PP_CAT(__MapTileServer_initialize_action_, mapper));                                        \
                                                                                                        \
        using HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_reduce_action_, mapper), _type) = \
         ::mt::server::MapTileServer<map_key, coordinate, mapper, tiler, reduce_value, input_key, provider>::reduce_action;  \
    HPX_REGISTER_ACTION(                                       \
        HPX_PP_CAT(HPX_PP_CAT(__MapTileServer_reduce_action_, mapper), _type),    \
        HPX_PP_CAT(__MapTileServer_reduce_action_, mapper));                                        \
                                                                                                        \
    typedef ::hpx::components::component<::mt::server::MapTileServer<map_key, coordinate, mapper, tiler, reduce_value, input_key, provider>> HPX_PP_CAT(__MapTileServer, mapper); \
    HPX_REGISTER_COMPONENT(HPX_PP_CAT(__MapTileServer, mapper)) \

#endif //MOBILITY_CPP_MAPTILESERVER_HPP
