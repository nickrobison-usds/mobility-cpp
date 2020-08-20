//
// Created by Nicholas Robison on 8/19/20.
//

#include "graph/BoostGraph.hpp"
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/degree_centrality.hpp>
#include <boost/graph/exterior_property.hpp>
#include <spdlog/spdlog.h>
#include <algorithm>

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

    void BoostGraph::add_vertex_impl(std::string_view actor) {
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
    BoostGraph::add_edge_impl(std::string_view movie, std::string_view actor1, std::string_view actor2) {
        const auto v1 = _actors.at(actor1.data());
        const auto v2 = _actors.at(actor2.data());
        const auto pair = boost::add_edge(v1, v2, _g);
        if (pair.second) {
            spdlog::debug("Adding edge {} between vertex {} and vertex {}", movie, actor1, actor2);
            _connecting_movie[pair.first] = movie;
        }
    }

    int BoostGraph::edge_count_impl() const {
        return boost::num_edges(_g);
    }

    int BoostGraph::vertex_count_impl() const {
        return boost::num_vertices(_g);
    }

    std::vector<int> BoostGraph::calculate_distance_impl(std::string_view actor) const {
        std::vector<int> bacon_number(vertex_count_impl());
        const auto src = _actors.at(actor.data());
        bacon_number[src] = 0;

        boost::breadth_first_search(_g, src, boost::visitor(distance_recorder(&bacon_number[0])));

        return bacon_number;
    }

    absl::flat_hash_map<std::string, unsigned long> BoostGraph::calculate_degree_centrality_impl() const {

        typedef boost::exterior_vertex_property<Graph, unsigned> CentralityProperty;
        typedef CentralityProperty::container_type CentralityContainer;
        typedef CentralityProperty::map_type CentralityMap;

        const auto vs = vertex_count();
        const auto vs2 = boost::num_vertices(_g);

        CentralityContainer cents(vs);
        CentralityMap cm(cents, _g);

//        std::vector<int> degree_centrality(vs);
        boost::all_degree_centralities(_g, cm);

        // Convert to output format

        absl::flat_hash_map<std::string, unsigned long> result;
        const auto vec_it = boost::vertices(_g);
        std::for_each(vec_it.first, vec_it.second, [&cm, &result, this](const auto c) {
            const auto name = this->_actor_name[c];
            const auto degree = cm[c];
            result.emplace(name, degree);
        });


        return result;
    }
}
