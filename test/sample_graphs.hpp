#ifndef SAMPLE_GRAPHS_HPP
#define SAMPLE_GRAPHS_HPP

#include "graph.hpp"

#include <tuple>
#include <vector>

// Returns this graph:
//
// 0 --- (1, {0, 1}) --- 1 --- (2, {1, 2}) --- 2
//
// Edge label: (w, SU)
// w - the weight of the edge
// SU - the set of slices on the edge

void
sample_graph1(graph &g, std::vector<vertex> &vs, std::vector<edge> &ve);

#endif /* SAMPLE_GRAPHS */
