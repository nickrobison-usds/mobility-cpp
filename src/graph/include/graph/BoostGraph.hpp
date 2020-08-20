//
// Created by Nicholas Robison on 8/19/20.
//

#ifndef MOBILITY_CPP_BOOSTGRAPH_HPP
#define MOBILITY_CPP_BOOSTGRAPH_HPP

#include "graph.hpp"
#include "impl/DistanceRecorder.hpp"
#include "impl/hasher.hpp"
#include <absl/container/flat_hash_map.h>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/degree_centrality.hpp>
#include <boost/graph/exterior_property.hpp>
#include <boost/graph/visitors.hpp>
#include <spdlog/spdlog.h>

namespace mcpp::graph {

    template<class NodeProperties, class EdgeProperties>
    class BoostGraph : public MCPPGraph<BoostGraph<NodeProperties, EdgeProperties>, NodeProperties, EdgeProperties> {

    public:
        explicit BoostGraph() = default;
        void add_vertex_impl(const NodeProperties &node) {
            const auto pair = _verticies.emplace(node, Vertex());
            if (pair.second) {
                spdlog::debug("Adding vertex {}", node);
                const auto vertex = boost::add_vertex(_g);
                _vertex_map[vertex] = node;
                pair.first->second = vertex;
            } else {
                spdlog::debug("Skipping existing vertex: {}", node);
            }

        }

        void add_edge_impl(const EdgeProperties &edge, const NodeProperties &source, const NodeProperties &target) {
            const auto v1 = _verticies[source];
            const auto v2 = _verticies[target];
            const auto pair = boost::add_edge(v1, v2, _g);
            if (pair.second) {
                spdlog::debug("Adding edge {} between vertex {} and vertex {}", edge, source, target);
                _edge_map[pair.first] = edge;
            }
        }

        [[nodiscard]] int edge_count_impl() const {
            return boost::num_edges(_g);
        }

        [[nodiscard]] int node_count_impl() const {
            return boost::num_vertices(_g);
        }

        [[nodiscard]] std::vector<int> calculate_distance_impl(const NodeProperties &start) const {
            std::vector<int> bacon_number(node_count_impl());
            const auto src = _verticies.at(start);
            bacon_number[src] = 0;

            boost::breadth_first_search(_g, src, boost::visitor(record_distance(&bacon_number[0])));
            return bacon_number;
        }

        [[nodiscard]] std::vector<std::pair<NodeProperties, unsigned long>> calculate_degree_centrality_impl() const {

            typedef boost::exterior_vertex_property<Graph, unsigned> CentralityProperty;
            typedef typename CentralityProperty::container_type CentralityContainer;
            typedef typename CentralityProperty::map_type CentralityMap;

            const auto vs = node_count_impl();
            CentralityContainer cents(vs);
            CentralityMap cm(cents, _g);
            boost::all_degree_centralities(_g, cm);

            // Convert to output format
            std::vector<std::pair<NodeProperties, unsigned long>> result;
            result.reserve(vs);
            const auto vec_it = boost::vertices(_g);
            std::for_each(vec_it.first, vec_it.second, [&cm, &result, this](const auto c) {
                const auto name = this->_vertex_map.at(c);
                const auto degree = cm[c];
                result.push_back(std::make_pair(name, degree));
            });

            return result;
        }

    private:
        typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, NodeProperties, EdgeProperties> Graph;
        typedef typename boost::vertex_bundle_type<Graph>::type node_map_t;
        typedef typename boost::edge_bundle_type<Graph>::type edge_map_t;
        typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
        typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
        typedef absl::flat_hash_map<node_map_t, Vertex> VertexMap;
        typedef absl::flat_hash_map<Vertex, node_map_t> VertexPropertiesMap;
        typedef absl::flat_hash_map<Edge, edge_map_t> EdgePropertiesMap;

        Graph _g;
        VertexMap _verticies;
        VertexPropertiesMap _vertex_map;
        EdgePropertiesMap _edge_map;
    };
}


#endif //MOBILITY_CPP_BOOSTGRAPH_HPP
