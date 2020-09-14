//
// Created by Nicholas Robison on 9/14/20.
//

#include "components/server/CountyShapefileServer.hpp"
#include <io/Shapefile.hpp>

namespace components::server {
    CountyShapefileServer::CountyShapefileServer(std::string shapefile) : _shapefile(
            io::Shapefile(std::move(shapefile)).openFile()) {
        // Not used
    }

    CountyShapefileServer::offset_type CountyShapefileServer::build_offsets() const {
        CountyShapefileServer::offset_type offsets;
        std::vector<std::string> geoids;

        std::size_t idx = 0;
        const auto layer = _shapefile->GetLayer(0);
        // Reset the filter, since we want everything
        layer->SetAttributeFilter(nullptr);
        const auto feature_count = layer->GetFeatureCount();
        offsets.reserve(feature_count);
        geoids.reserve(feature_count);

        std::for_each(layer->begin(), layer->end(), [&geoids](auto &feature) {
            geoids.push_back(feature->GetFieldAsString("GEOID"));
        });
        // FIXME: For some reason, this fails the address sanitizer, which is frustrating
        std::sort(geoids.begin(), geoids.end());
        std::for_each(geoids.begin(), geoids.end(), [&offsets, &idx](const auto &geoid) {
            offsets.emplace_back(geoid, idx);
            idx++;
        });

        return offsets;
    }
}