//
// Created by Nicholas Robison on 6/1/20.
//

#ifndef MOBILITY_CPP_TILESERVER_HPP
#define MOBILITY_CPP_TILESERVER_HPP

#include <hpx/include/serialization.hpp>
#include <hpx/include/components.hpp>
#include <blaze/math/DynamicMatrix.h>

#include <cstddef>
#include "../TileDimension.hpp"

namespace components {


    class TileServer : public hpx::components::component_base<TileServer> {

    public:
        TileServer() : _dim() {
            // Not used
        }

        void init(std::vector<std::string> filenames, TileDimension const &dim, std::size_t num_nodes);

        HPX_DEFINE_COMPONENT_ACTION(TileServer, init);

    private:
        components::TileDimension _dim;
        std::vector<blaze::DynamicMatrix<uint16_t>> _visit_matrix;
        std::vector<blaze::DynamicMatrix<double>> _distance_matrix;
    };
}


#endif //MOBILITY_CPP_TILESERVER_HPP
