//
// Created by Nicholas Robison on 5/22/20.
//

#ifndef MOBILITY_CPP_LOCATIONJOINERSERVER_HPP
#define MOBILITY_CPP_LOCATIONJOINERSERVER_HPP

#include <string>
#include "../include/components/data.hpp"
#include <io/shapefile.hpp>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>
#include <hpx/include/serialization.hpp>
#include <utility>

namespace components::server {

    class LocationJoinerServer : public hpx::components::component_base<LocationJoinerServer> {

    public:

        LocationJoinerServer(std::vector<std::string> csv_files, std::string shapefile) : _csv_file(std::move(csv_files)), _shapefile(std::move(shapefile)) {};

        std::vector<safegraph_location> invoke() const;

        HPX_DEFINE_COMPONENT_ACTION(LocationJoinerServer, invoke);

    private:
        std::vector<std::string> _csv_file;
        std::string _shapefile;

        hpx::future<int> read_shapefile(std::shared_ptr<GDALDataset> &ptr) const;
        hpx::future<std::vector<safegraph_location>> read_csv(std::string csv_file) const;
    };
}
HPX_REGISTER_ACTION_DECLARATION(::components::server::LocationJoinerServer::invoke_action,
                                location_joiner_invoke_action);
#endif //MOBILITY_CPP_LOCATIONJOINERSERVER_HPP
