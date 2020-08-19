//
// Created by Nicholas Robison on 8/19/20.
//

#include "graph/BoostGraph.hpp"
#include <boost/graph/breadth_first_search.hpp>
#include <spdlog/spdlog.h>

namespace mcpp::graph {

    template<typename DistanceMap>
    struct distance_recorder : public boost::default_bfs_visitor {

        explicit distance_recorder(DistanceMap dist) : d(dist) {}

        template<typename Edge, typename Graph>
        void tree_edge(Edge e, const Graph &g) const {
            typename boost::graph_traits<Graph>::vertex_descriptor u = source(e, g),
                    v = target(e, g);
            d[v] = d[u] + 1;
        }

    private:
        DistanceMap d;
    };

    // Convenience function
    template<typename DistanceMap>
    distance_recorder<DistanceMap> record_distance(DistanceMap d) {
        return distance_recorder<DistanceMap>(d);
    }

    BoostGraph::BoostGraph() {
        // Create the graph
        _actor_name = boost::get(boost::vertex_name, _g);
        _connecting_movie = boost::get(boost::edge_name, _g);
    }

    void BoostGraph::add_vertex(const std::string_view actor) {
        // Try to insert into the map, unless we've already seen it.

        const auto pair = _actors.emplace(actor, Vertex());
        // If inserted, then we need to insert into graph itself
        if (pair.second) {
            spdlog::debug("Adding vertex {}", actor);
            const auto vertex = boost::add_vertex(_g);
            _actor_name[vertex] = actor;
            pair.first->second = vertex;
        } else {
            spdlog::debug("Skipping existing vertex: {}", actor);
        }

    }

    void
    BoostGraph::add_edge(const std::string_view movie, const std::string_view actor1, const std::string_view actor2) {
        const auto v1 = _actors.at(actor1.data());
        const auto v2 = _actors.at(actor2.data());
        const auto pair = boost::add_edge(v1, v2, _g);
        if (pair.second) {
            spdlog::debug("Adding edge {} between vertex {} and vertex {}", movie, actor1, actor2);
            _connecting_movie[pair.first] = movie;
        }
    }

    int BoostGraph::edge_count() const {
        return boost::num_edges(_g);
    }

    int BoostGraph::vertex_count() const {
        return boost::num_vertices(_g);
    }

    std::vector<int> BoostGraph::calculate_distance(const std::string_view actor) const {
        std::vector<int> bacon_number(vertex_count());
        const auto src = _actors.at(actor.data());
        bacon_number[src] = 0;

        boost::breadth_first_search(_g, src, boost::visitor(distance_recorder(&bacon_number[0])));

        return bacon_number;
    }
}
