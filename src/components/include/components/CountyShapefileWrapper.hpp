//
// Created by Nicholas Robison on 9/14/20.
//

#ifndef MOBILITY_CPP_COUNTYSHAPEFILEWRAPPER_HPP
#define MOBILITY_CPP_COUNTYSHAPEFILEWRAPPER_HPP

#include "components/server/CountyShapefileServer.hpp"
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>

namespace components {
class CountyShapefileWrapper: public hpx::components::client_base<CountyShapefileWrapper, components::server::CountyShapefileServer>  {

public:
    explicit CountyShapefileWrapper(hpx::future<hpx::id_type> &&f) : client_base(std::move(f)) {};

    explicit CountyShapefileWrapper(hpx::id_type &&f) : client_base(std::move(f)) {};

    explicit CountyShapefileWrapper(const std::string& shapefile);
    hpx::future<server::CountyShapefileServer::offset_type> build_offsets() const;

    };
}





#endif //MOBILITY_CPP_COUNTYSHAPEFILEWRAPPER_HPP
