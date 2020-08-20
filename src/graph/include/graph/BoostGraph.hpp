//
// Created by Nicholas Robison on 8/19/20.
//

#ifndef MOBILITY_CPP_BOOSTGRAPH_HPP
#define MOBILITY_CPP_BOOSTGRAPH_HPP

#include "graph.hpp"
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/visitors.hpp>

namespace mcpp::graph {

    class BoostGraph : public MCPPGraph<BoostGraph> {

    public:
        explicit BoostGraph();
        void add_vertex_impl(std::string_view actor);
        void add_edge_impl(std::string_view movie, std::string_view actor1, std::string_view actor2);
        [[nodiscard]] int edge_count_impl() const;
        [[nodiscard]] int vertex_count_impl() const;
        [[nodiscard]] std::vector<int> calculate_distance_impl(std::string_view actor) const;
        [[nodiscard]] absl::flat_hash_map<std::string, unsigned long> calculate_degree_centrality_impl() const;

    private:
        typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::property<boost::vertex_name_t, std::string>, boost::property<boost::edge_name_t, std::string>> Graph;
        typedef boost::property_map<Graph, boost::vertex_name_t>::type actor_name_map_t;
        typedef boost::property_map<Graph, boost::edge_name_t>::type movie_name_map_t;
        typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;
        typedef absl::flat_hash_map<std::string, Vertex> NameVertexMap;
        Graph _g;
        NameVertexMap _actors;
        actor_name_map_t _actor_name;
        movie_name_map_t _connecting_movie;
    };
}


#endif //MOBILITY_CPP_BOOSTGRAPH_HPP
