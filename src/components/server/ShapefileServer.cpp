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
        auto res = std::reduce(layer->begin(), layer->end(), std::vector<std::pair<std::string, OGRPoint>>(), [](auto &acc, auto &feature) {
            const auto geoid = feature->GetFieldAsString("GEOID");
            OGRPoint centroid;
            feature->GetGeometryRef()->Centroid(&centroid);
            acc.emplace_back(geoid, centroid);
            return acc;
        });

        return res;
    }
}