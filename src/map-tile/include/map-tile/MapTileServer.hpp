//
// Created by Nicholas Robison on 7/19/20.
//

#ifndef MOBILITY_CPP_MAPTILESERVER_HPP
#define MOBILITY_CPP_MAPTILESERVER_HPP

#include <algorithm>
#include <utility>
#include <vector>
#include <io/csv_reader.hpp>

#include "Context.hpp"
#include "../../src/FileProvider.hpp"

using namespace std;

namespace mt::server {
    template<
            class MapKey,
            class ReduceKey,
            template<typename = MapKey, typename = ReduceKey> class Mapper,
            class InputKey = std::string,
            template<typename = InputKey> class Provider = io::FileProvider
            >
    class MapTileServer {
    public:
        explicit MapTileServer(vector<string> files) : _files(std::move(files)) {
            // Not used
        }

        void tile() {
            // Instantiate the context
            const auto ctx = make_shared<mt::Context<MapKey, false>>();
            // Load the CSV files
            // Let's do it all in memory for right now
            for_each(_files.begin(), _files.end(), [&ctx](const string &filename) {
                Provider<InputKey> provider(filename);
                vector<InputKey> keys = provider.provide();
                // Map each one
                Mapper<MapKey, ReduceKey> mapper;
                // Setup

                //Map
                for_each(keys.begin(), keys.end(), [&mapper, &ctx](const auto &key) {
                    mapper.map(*ctx, key);
                });
            });

            // Now, tile
        }

    private:
        const vector<string> _files;

    };
}


#endif //MOBILITY_CPP_MAPTILESERVER_HPP
