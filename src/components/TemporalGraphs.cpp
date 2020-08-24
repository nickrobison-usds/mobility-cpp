//
// Created by Nicholas Robison on 8/21/20.
//

#include "components/TemporalGraphs.hpp"
#include <spdlog/spdlog.h>

namespace components {

    TemporalGraphs::TemporalGraphs(std::size_t graphs) : _locks(graphs), _graphs(graphs) {
        // Not used
    }

    void TemporalGraphs::insert(std::size_t time, const v2 &value) {
        absl::MutexLock lock(&_locks.at(time));
        auto &graph = _graphs.at(time);
        graph.add_edge(value.visits, value.location_cbg, value.location_cbg);
    }

    std::vector<std::pair<std::string, unsigned long>>
    TemporalGraphs::calculate_degree_centrality(const std::size_t time) const {
        const auto &graph = _graphs.at(time);
        return graph.calculate_degree_centrality();
    }
}
