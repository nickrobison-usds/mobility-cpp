//
// Created by Nicholas Robison on 5/22/20.
//

#ifndef MOBILITY_CPP_JOINEDLOCATIONSERVER_HPP
#define MOBILITY_CPP_JOINEDLOCATIONSERVER_HPP

#include <string>
#include "../include/components/sl.hpp"
#include "../include/components/jl.hpp"
#include <io/shapefile.hpp>
#include <io/parquet.hpp>
#include <absl/container/flat_hash_map.h>
#include <hpx/include/actions.hpp>
#include <hpx/include/lcos.hpp>
#include <hpx/include/components.hpp>

#include <utility>

namespace components::server {

    class JoinedLocationServer : public hpx::components::component_base<JoinedLocationServer> {

    public:

        JoinedLocationServer(std::vector<std::string> csv_files, std::string shapefile);

        std::vector<safegraph_location> invoke();
        joined_location find_location(const std::string &safegraph_place_id);
        HPX_DEFINE_COMPONENT_ACTION(JoinedLocationServer, invoke);
        HPX_DEFINE_COMPONENT_ACTION(JoinedLocationServer, find_location);

    private:
        const std::vector<std::string> _csv_file;
        const std::string _shapefile;
        const absl::flat_hash_map<std::string, joined_location> _cache;
        const io::Parquet _parquet;

        hpx::future<int> read_shapefile(std::shared_ptr<GDALDataset> &ptr) const;
        hpx::future<std::vector<safegraph_location>> read_csv(std::string csv_file) const;
        absl::flat_hash_map<std::string, joined_location> loadLocationCache() const;
    };
}
HPX_REGISTER_ACTION_DECLARATION(::components::server::JoinedLocationServer::invoke_action,
                                location_joiner_invoke_action);
HPX_REGISTER_ACTION_DECLARATION(::components::server::JoinedLocationServer::find_location_action,
                                location_joiner_find_location_action);
#endif //MOBILITY_CPP_JOINEDLOCATIONSERVER_HPP
