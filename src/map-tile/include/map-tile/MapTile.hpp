//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_MAPTILE_HPP
#define MOBILITY_CPP_MAPTILE_HPP

#include "client/MapTileClient.hpp"
#include "coordinates/LocaleLocator.hpp"
#include "coordinates/LocaleTiler.hpp"
#include <boost/filesystem.hpp>
#include <hpx/future.hpp>
#include <range/v3/view/cycle.hpp>
#include <range/v3/view/zip.hpp>
#include <range/v3/algorithm/for_each.hpp>
#include <shared/DirectoryUtils.hpp>
#include <spdlog/spdlog.h>
#include <numeric>
#include <unordered_map>
#include <vector>

namespace fs = boost::filesystem;

namespace mt {

    template<
            class MapKey,
            class Coordinate,
            class Mapper,
            class Tiler,
            class ReduceValue
    >
    class MapTile {
    public:
        typedef typename client::MapTileClient<MapKey, Coordinate, Mapper, Tiler, ReduceValue> client_type;

        explicit MapTile(const Coordinate &min, const Coordinate &max,
                         const std::array<std::size_t, Coordinate::dimensions> &stride, const fs::path &directory,
                         const std::string_view regex, const std::map<std::string, std::string> &config_values) {
            // Tile the input space
            const auto locales = hpx::find_all_localities();
            spdlog::info("Executing on {} locales", locales.size());

            const auto tiles = coordinates::LocaleTiler::tile<Coordinate>(min,
                                                                          max, stride);

            spdlog::debug("Partitioned into {} tiles", tiles.size());
            const coordinates::LocaleLocator<Coordinate> locator(tiles);

            if (locales.size() != tiles.size()) {
                spdlog::warn("Cannot execute {} tiles in {} locales.", tiles.size(), locales.size());
            }

            // Partition the input files, try one for each tile
            spdlog::info("Reading files from: {}, with filter: {}", directory, regex);
            const auto files = shared::DirectoryUtils::partition_files(
                    directory.string(),
                    tiles.size(),
                    regex);

            const auto num_files = std::accumulate(files.begin(), files.end(), 0, [](auto acc, const auto &f) {
                return acc + f.size();
            });
            spdlog::info("Processing {} input files", num_files);

            // Create a queue that will let us handle cases where we have more servers than input files.
            // If the queue is empty, we'll simply pass an empty vector to the server
            queue<vector<fs::directory_entry>, deque<vector<fs::directory_entry>>> file_queue(
                    deque<vector<fs::directory_entry>>(files.begin(), files.end()));

            // Create a a server for each tile, cycling through the locales and files
            const auto locale_range = ranges::views::cycle(locales);
            const auto z = ranges::views::zip(locale_range, tiles);

            std::vector<client_type> clients;

            ranges::for_each(
                    z,
                    [&clients, &locator, &config_values, &file_queue](
                            const auto &pair) {
                        vector<string> file_strs;

                        if (!file_queue.empty()) {
                            const auto files = file_queue.front();
                            transform(files.begin(), files.end(), back_inserter(file_strs), [](const auto &f) {
                                return f.path().string();
                            });
                            file_queue.pop();
                        }

                        const auto tile = get<1>(pair);
                        spdlog::debug("Creating server on locale {}", get<0>(pair));
                        client_type server(
                                get<0>(pair), locator,
                                tile,
                                config_values,
                                file_strs);
                        clients.push_back(std::move(server));
                    });

            _clients = clients;
        }

        std::vector<hpx::future<void>> initialize() {
            vector<hpx::future<void>> results;
            std::transform(_clients.begin(), _clients.end(), std::back_inserter(results), [](auto &server) {
                return std::move(server.initialize());
            });

            return results;
        }

        std::vector<hpx::future<void>> tile() {
            vector<hpx::future<void>> results;
            std::transform(_clients.begin(), _clients.end(), std::back_inserter(results), [](auto &server) {
                return std::move(server.tile());
            });

            return results;
        }

        std::vector<hpx::future<void>> compute() {
            vector<hpx::future<void>> results;
            std::transform(_clients.begin(), _clients.end(), std::back_inserter(results), [](auto &server) {
                return std::move(server.compute());
            });

            return results;
        }

        std::vector<hpx::future<ReduceValue>> reduce() {
            vector<hpx::future<ReduceValue>> results;
            std::transform(_clients.begin(), _clients.end(), std::back_inserter(results), [](auto &mt) {
                return std::move(mt.reduce());
            });

            return results;
        }

    private:
        std::vector<client_type> _clients;
    };


}

#endif //MOBILITY_CPP_MAPTILE_HPP
