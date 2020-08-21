//
// Created by Nicholas Robison on 8/21/20.
//

#ifndef MOBILITY_CPP_TEMPORALGRAPHS_HPP
#define MOBILITY_CPP_TEMPORALGRAPHS_HPP

#include <shared/data.hpp>
#include <absl/synchronization/mutex.h>
#include <graph/BoostGraph.hpp> // Make sure this comes after data.hpp, which pulls in the hpx config header.
#include <vector>

namespace components {

    class TemporalGraphs {
    public:
        explicit TemporalGraphs(std::size_t graphs);

        void insert(std::size_t time, const v2 &value);

        std::vector<std::pair<std::string, unsigned long>> calculate_degree_centrality(const std::size_t time) const;

    private:
        typedef mcpp::graph::BoostGraph<std::string, std::uint32_t> Graph;
        std::vector<Graph> _graphs;
        std::vector<absl::Mutex> _locks;
    };

}


#endif //MOBILITY_CPP_TEMPORALGRAPHS_HPP
