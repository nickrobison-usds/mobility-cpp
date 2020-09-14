//
// Created by Nicholas Robison on 6/3/20.
//

#ifndef MOBILITY_CPP_CBGSHAPEFILEWRAPPER_HPP
#define MOBILITY_CPP_CBGSHAPEFILEWRAPPER_HPP

#include "components/server/CBGShapefileServer.hpp"
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>

namespace components {
    class CBGShapefileWrapper : public hpx::components::client_base<CBGShapefileWrapper, components::server::CBGShapefileServer> {

    public:
        explicit CBGShapefileWrapper(hpx::future<hpx::id_type> &&f) : client_base(std::move(f)) {};

        explicit CBGShapefileWrapper(hpx::id_type &&f) : client_base(std::move(f)) {};

        explicit CBGShapefileWrapper(const std::string& shapefile);



        hpx::future<std::vector<std::pair<std::string, OGRPoint>>> get_centroids(const std::vector<std::string> &geoids) const;
        hpx::future<server::CBGShapefileServer::offset_type> build_offsets() const;

    };
}

#endif //MOBILITY_CPP_CBGSHAPEFILEWRAPPER_HPP
