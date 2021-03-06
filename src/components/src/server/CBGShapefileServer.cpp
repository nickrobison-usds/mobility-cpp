//
// Created by Nicholas Robison on 6/3/20.
//

#include "components/server/CBGShapefileServer.hpp"
#include <shared/debug.hpp>
#include "shared/constants.hpp"
#include "spdlog/spdlog.h"

#include <algorithm>

namespace components::server {


    CBGShapefileServer::CBGShapefileServer(std::string shapefile) : _shapefile(
            io::Shapefile(std::move(shapefile)).openFile()), _centroid_map(build_centroid_map()) {
        // Not used
    }

    std::vector<std::pair<std::string, OGRPoint>> CBGShapefileServer::get_centroids(const vector<std::string> &geoids) {
        const auto dp = shared::DebugInterval::create_debug_point(shared::SignPostCode::GET_CENTROIDS);

        std::vector<std::pair<std::string, OGRPoint>> centroids;
        centroids.reserve(geoids.size());

        std::transform(geoids.begin(), geoids.end(), std::back_inserter(centroids), [this](const auto &geoid) {
            try {
                const auto centroid = _centroid_map.at(geoid);
                return std::make_pair(geoid, centroid);
            } catch (std::exception &e) {
                spdlog::error("Cannot find point for place: {}", geoid);
                throw e;
            }

        });

        dp.stop();
        return centroids;
    }

    CBGShapefileServer::offset_type CBGShapefileServer::build_offsets() const {
        CBGShapefileServer::offset_type offsets;
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

    CBGShapefileServer::centroid_map CBGShapefileServer::build_centroid_map() {
        spdlog::debug("Building centroid map");
        centroid_map map;

        const auto layer = _shapefile->GetLayer(0);
        // Reset the filter, since we want everything
        layer->SetAttributeFilter(nullptr);

        std::for_each(layer->begin(), layer->end(), [&map](auto &feature) {
            const auto geoid = feature->GetFieldAsString("GEOID");
            OGRPoint centroid;
            feature->GetGeometryRef()->Centroid(&centroid);
            map.emplace(geoid, centroid);
        });

        spdlog::debug("Finished building centroid map");
        return map;
    }

}
