//
// Created by Nicholas Robison on 8/18/20.
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file

#include <graph/BoostGraph.hpp>
#include <absl/strings/str_split.h>
#include "catch2/catch.hpp"
#include <algorithm>
#include <fstream>
#include <vector>

TEST_CASE("Actors", "[graph]") {

    mcpp::graph::BoostGraph g;

    // Open the file and read each line
    std::ifstream datafile("data/kevin-bacon.dat");
    for (std::string line; std::getline(datafile, line);) {
        const std::vector<std::string> split = absl::StrSplit(line, ';');
        g.add_vertex(split[0]);
        g.add_vertex(split[2]);
        g.add_edge(split[1], split[0], split[2]);
    }

    REQUIRE(g.edge_count() == 50);
    REQUIRE(g.vertex_count() == 51);

    const auto bn = g.calculate_distance("Kevin Bacon");
    REQUIRE(std::accumulate(bn.begin(), bn.end(), 0) == 112);

}