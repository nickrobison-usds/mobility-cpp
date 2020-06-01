//
// Created by Nicholas Robison on 6/1/20.
//

#include "TileServer.hpp"

namespace components {
    void TileServer::init(std::vector<std::string> filenames, const components::TileDimension &dim,
                                      std::size_t num_nodes) {
        _dim = dim;
    }
}
