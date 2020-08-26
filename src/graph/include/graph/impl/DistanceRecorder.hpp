//
// Created by Nicholas Robison on 8/20/20.
//

#ifndef MOBILITY_CPP_DISTANCERECORDER_HPP
#define MOBILITY_CPP_DISTANCERECORDER_HPP

#include <boost/graph/breadth_first_search.hpp>

namespace mcpp::graph {

    template<typename DistanceMap>
    struct DistanceRecorder : public boost::default_bfs_visitor {

        explicit DistanceRecorder(DistanceMap dist) : d(dist) {}

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
    DistanceRecorder<DistanceMap> record_distance(DistanceMap d) {
        return DistanceRecorder<DistanceMap>(d);
    }
}

#endif //MOBILITY_CPP_DISTANCERECORDER_HPP
