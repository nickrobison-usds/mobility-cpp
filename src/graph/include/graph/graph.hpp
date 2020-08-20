//
// Created by Nicholas Robison on 8/18/20.
//

#ifndef MOBILITY_CPP_GRAPH_HPP
#define MOBILITY_CPP_GRAPH_HPP

#include <absl/container/flat_hash_map.h>
#include <string_view>
#include <vector>

namespace mcpp::graph {

    template<class Backend>
    class MCPPGraph {
    public:
        void add_edge(std::string_view movie, std::string_view actor1, std::string_view actor2) {
            static_cast<Backend &> (*this).add_edge_impl(movie, actor1, actor2);
        }

        void add_vertex(std::string_view actor) {
            static_cast<Backend &> (*this).add_vertex_impl(actor);
        }

        [[nodiscard]] int edge_count() const {
            return static_cast<Backend const &> (*this).edge_count_impl();
        }

        [[nodiscard]] int vertex_count() const {
            return static_cast<Backend const &> (*this).vertex_count_impl();
        }

        [[nodiscard]] std::vector<int> calculate_distance(std::string_view actor) const {
            return static_cast<Backend const &> (*this).calculate_distance_impl(actor);
        }

        [[nodiscard]] absl::flat_hash_map<std::string, unsigned long> calculate_degree_centrality() const {
            return static_cast<Backend const &> (*this).calculate_degree_centrality_impl();
        }

    protected:
        MCPPGraph() = default;
    };
}

#endif //MOBILITY_CPP_GRAPH_HPP
