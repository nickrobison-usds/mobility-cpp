//
// Created by Nicholas Robison on 6/3/20.
//

#include "ShapefileServer.hpp"
#include <absl/strings/str_join.h>

#include <algorithm>

namespace components::server {
    ShapefileServer::ShapefileServer(std::string shapefile): _shapefile(io::Shapefile(std::move(shapefile)).openFile()) {
        // Not used
    }

    std::vector<std::pair<std::string, OGRPoint>> ShapefileServer::get_centroids(const vector<std::string> &geoids) {
        const auto layer = _shapefile->GetLayer(0);

        // Join all of the CBGs into a single query statement
        const auto joined = absl::StrJoin(geoids, "','");
        const auto query = absl::StrCat("GEOID IN ('", joined, "')");

        layer->SetAttributeFilter(query.c_str());
        std::vector<std::pair<std::string, OGRPoint>> res;
        std::for_each(layer->begin(), layer->end(), [&res](OGRFeatureUniquePtr &feature) {
            const auto geoid = feature->GetFieldAsString("GEOID");
            OGRPoint centroid;
            feature->GetGeometryRef()->Centroid(&centroid);
            res.emplace_back(geoid, centroid);
        });

        return res;
    }

    ShapefileServer::offset_type ShapefileServer::build_offsets() const {
        ShapefileServer::offset_type offsets;
        std::vector<std::string> geoids;

        std::size_t idx = 0;
        const auto layer = _shapefile->GetLayer(0);
        // Reset the filter, since we want everything
        layer->SetAttributeFilter(nullptr);
        offsets.reserve(layer->GetFeatureCount());
        geoids.reserve(layer->GetFeatureCount());

        std::for_each(layer->begin(), layer->end(), [&geoids](auto &feature) {
            geoids.push_back(feature->GetFieldAsString("GEOID"));
        });
        std::sort(geoids.begin(), geoids.end());
        std::for_each(geoids.begin(), geoids.end(), [&offsets, &idx](const auto &geoid) {
            offsets.emplace_back(geoid, idx);
            idx++;
        });

        return offsets;
    }

}