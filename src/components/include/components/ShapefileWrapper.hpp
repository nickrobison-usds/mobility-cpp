//
// Created by Nicholas Robison on 6/3/20.
//

#ifndef MOBILITY_CPP_SHAPEFILEWRAPPER_HPP
#define MOBILITY_CPP_SHAPEFILEWRAPPER_HPP

#include "../../server/ShapefileServer.hpp"
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>

namespace components {
    class ShapefileWrapper : public hpx::components::client_base<ShapefileWrapper, components::server::ShapefileServer> {

    public:
        explicit ShapefileWrapper(hpx::future<hpx::id_type> &&f) : client_base(std::move(f)) {};

        explicit ShapefileWrapper(hpx::id_type &&f) : client_base(std::move(f)) {};

        explicit ShapefileWrapper(std::string shapefile);

        hpx::future<std::vector<std::pair<std::string, OGRPoint>>> get_centroids(const std::vector<std::string> &geoids) const;
        hpx::future<server::ShapefileServer::offset_type> build_offsets() const;

    };
}

#endif //MOBILITY_CPP_SHAPEFILEWRAPPER_HPP
