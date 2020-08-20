//
// Created by Nicholas Robison on 8/18/20.
//

#ifndef MOBILITY_CPP_GRAPH_HPP
#define MOBILITY_CPP_GRAPH_HPP

#include <absl/container/flat_hash_map.h>
#include <string_view>
#include <vector>

namespace mcpp::graph {

    template<class Backend, class NodeProperties, class EdgeProperties>
    class MCPPGraph {
    public:
        void add_edge(const EdgeProperties &edge, const NodeProperties &source, const NodeProperties &target) {
            static_cast<Backend &> (*this).add_edge_impl(edge, source, target);
        }

        void add_vertex(const NodeProperties &node) {
            static_cast<Backend &> (*this).add_vertex_impl(node);
        }

        [[nodiscard]] int edge_count() const {
            return static_cast<Backend const &> (*this).edge_count_impl();
        }

        [[nodiscard]] int node_count() const {
            return static_cast<Backend const &> (*this).node_count_impl();
        }

        [[nodiscard]] std::vector<int> calculate_distance(const NodeProperties &start) const {
            return static_cast<Backend const &> (*this).calculate_distance_impl(start);
        }

        [[nodiscard]] absl::flat_hash_map<NodeProperties, unsigned long> calculate_degree_centrality() const {
            return static_cast<Backend const &> (*this).calculate_degree_centrality_impl();
        }

    protected:
        MCPPGraph() = default;
    };
}

#endif //MOBILITY_CPP_GRAPH_HPP
