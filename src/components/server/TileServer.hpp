//
// Created by Nicholas Robison on 6/1/20.
//

#ifndef MOBILITY_CPP_TILESERVER_HPP
#define MOBILITY_CPP_TILESERVER_HPP

#include "../TileDimension.hpp"
#include <io/shapefile.hpp>
#include <hpx/include/serialization.hpp>
#include <hpx/include/components.hpp>
#include <blaze/math/DynamicMatrix.h>

#include <cstddef>

namespace components::server {

    struct sl2 {
        string safegraph_place_id;
        string parent_safegraph_place_id;
        string location_name;
        string safegraph_brand_ids;
        string brands;
        string top_category;
        string sub_category;
        string naics_code;
        std::uint64_t cbg;
        double latitude;
        double longitude;
    };

    class TileServer : public hpx::components::component_base<TileServer> {

    public:
        typedef blaze::DynamicMatrix<uint16_t> visit_matrix;
        typedef blaze::DynamicMatrix<double> distance_matrix;

        TileServer();

        void init(const std::string &filename, const TileDimension &dim, std::size_t num_nodes);

        HPX_DEFINE_COMPONENT_ACTION(TileServer, init);

    private:
        const GDALDatasetUniquePtr _p;
        components::TileDimension _dim;
        std::vector<visit_matrix> _visits;
        std::vector<distance_matrix> _distances;


        std::vector<sl2> extract_rows(const std::string &filename) const;
        std::tuple<std::uint64_t , std::uint64_t, double> computeDistance(const sl2 &row) const;
        std::vector<std::uint16_t> expandRow(const sl2 &row) const;
    };
}

HPX_REGISTER_ACTION_DECLARATION(::components::server::TileServer::init_action, tile_server_init_action);

#endif //MOBILITY_CPP_TILESERVER_HPP
