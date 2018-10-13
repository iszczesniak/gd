// Include graph.hpp here, because it has the Vertex and Edge
// templates, which are needed by generic_dijkstra.hpp.

#include "graph.hpp"

#include "accountant.hpp"
#include "adaptive_units.hpp"
#include "generic_dijkstra.hpp"
#include "generic_label.hpp"
#include "generic_label_creator.hpp"
#include "generic_tracer.hpp"
#include "utils.hpp"

#include <boost/optional.hpp>

#include <iostream>

#define BOOST_TEST_MODULE dijkstra
#include <boost/test/unit_test.hpp>

using namespace std;

typedef generic_solution<graph, COST, CU> sol_type;

// Make sure that we can't find a path if there are no slices.  The
// graph is a simple case of two nodes and a single link.
BOOST_AUTO_TEST_CASE(dijkstra_test_1)
{
  // Play.  Safe.
  adaptive_units<COST>::longest_shortest_path(100);
  routing::set_st(routing::st_t::first);

  graph g(2);
  vertex src = *(boost::vertices(g).first);
  vertex dst = *(boost::vertices(g).first + 1);
  boost::add_edge(src, dst, g);
  set_units(g, 2);

  generic_label_creator<graph, COST, CU> c(g, 3);
  generic_label<graph, COST, CU> l(0, {0, 2}, edge(), src);
  sol_type S, Q;
  dijkstra(g, S, Q, l, c, dst);
  // The tracer.
  generic_tracer<graph, cupath, sol_type, CU> t(g, 3);
  // Get the path.
  auto op = trace(S, dst, l, t);

  // There are no results for dst.
  BOOST_CHECK(!op);
}

// Make sure that we can find a path if there are slices.  The graph
// is a simple case of two nodes and a single link.
BOOST_AUTO_TEST_CASE(dijkstra_test_2)
{
  // Play.  Safe.
  adaptive_units<COST>::longest_shortest_path(100);
  routing::set_st(routing::st_t::first);

  graph g(2);
  vertex src = *(boost::vertices(g).first);
  vertex dst = *(boost::vertices(g).first + 1);
  edge e = boost::add_edge(src, dst, g).first;
  boost::get(boost::edge_su, g, e) = {{0, 3}};
  boost::get(boost::edge_weight, g, e) = 1;

  generic_label_creator<graph, COST, CU> c(g, 3);
  generic_label<graph, COST, CU> l(0, {0, 3}, edge(), src);
  sol_type S, Q;
  dijkstra(g, S, Q, l, c, dst);
  // The tracer.
  generic_tracer<graph, cupath, sol_type, CU> t(g, 3);
  // Get the path.
  auto op = trace(S, dst, l, t);

  // The path has one edge and three units.
  BOOST_CHECK(op.value() == cupath(CU(0, 3), {e}));
}

// Make sure we can establish a path with a worse cost than the
// shortest path, but with a different su.
BOOST_AUTO_TEST_CASE(dijkstra_test_3)
{
  // Play.  Safe.
  adaptive_units<COST>::longest_shortest_path(100);
  routing::set_st(routing::st_t::first);

  graph g(3);
  vertex src = *(boost::vertices(g).first);
  vertex mid = *(boost::vertices(g).first + 1);
  vertex dst = *(boost::vertices(g).first + 2);
  edge e1 = boost::add_edge(src, mid, g).first;
  edge e2 = boost::add_edge(src, mid, g).first;
  edge e3 = boost::add_edge(mid, dst, g).first;

  // Props of edge e1.
  boost::get(boost::edge_weight, g, e1) = 1;
  boost::get(boost::edge_su, g, e1) = {{0, 2}};

  // Props of edge e2.
  boost::get(boost::edge_weight, g, e2) = 2;
  boost::get(boost::edge_su, g, e2) = {{1, 3}};

  // Props of edge e3.
  boost::get(boost::edge_weight, g, e3) = 1;
  boost::get(boost::edge_su, g, e3) = {{1, 3}};

  generic_label_creator<graph, COST, CU> c(g, 2);
  generic_label<graph, COST, CU> l(0, {0, 3}, edge(), src);
  sol_type S, Q;
  dijkstra(g, S, Q, l, c, dst);
  // The tracer.
  generic_tracer<graph, cupath, sol_type, CU> t(g, 2);
  // Get the path.
  auto op = trace(S, dst, l, t);

  // We found the path - it should be e2, e3.
  BOOST_CHECK(op.value() == cupath({1, 3}, {e2, e3}));
}

// Make sure we don't remember at node mid the results for edge e1
// that led to that node with a worse cost than edge e2 and with the
// same slices.
BOOST_AUTO_TEST_CASE(dijkstra_test_4)
{
  // Play.  Safe.
  adaptive_units<COST>::longest_shortest_path(100);
  routing::set_st(routing::st_t::first);

  graph g(3);
  vertex src = *(vertices(g).first);
  vertex mid = *(vertices(g).first + 1);
  vertex dst = *(vertices(g).first + 2);
  edge e1 = add_edge(src, mid, g).first;
  edge e2 = add_edge(src, mid, g).first;
  edge e3 = add_edge(mid, dst, g).first;

  // Props of edge e1.
  boost::get(boost::edge_weight, g, e1) = 2;
  boost::get(boost::edge_su, g, e1) = {{0, 1}};

  // Props of edge e2.
  boost::get(boost::edge_weight, g, e2) = 1;
  boost::get(boost::edge_su, g, e2) = {{0, 1}};

  // Props of edge e3.
  boost::get(boost::edge_weight, g, e3) = 2;
  boost::get(boost::edge_su, g, e3) = {{0, 1}};

  generic_label_creator<graph, COST, CU> c(g, 1);
  generic_label<graph, COST, CU> l(0, {0, 3}, edge(), src);
  sol_type S, Q;
  dijkstra(g, S, Q, l, c, dst);
  // The tracer.
  generic_tracer<graph, cupath, sol_type, CU> t(g, 1);
  // Get the path.
  auto op = trace(S, dst, l, t);

  // We found the path of two labels.
  BOOST_CHECK(op.value() == cupath({0, 1}, {e2, e3}));
  // We don't remember the results for edge e1.
  BOOST_CHECK(S[mid].size() == 1);
}

// Make sure we remember at node mid the results for both edge e1 and
// e2.  e2 offers the shortest path with slice 0, and e1 offers the
// two slices: 0 and 1 albeit at a higher cost.
BOOST_AUTO_TEST_CASE(dijkstra_test_5)
{
  // Play.  Safe.
  adaptive_units<COST>::longest_shortest_path(100);
  routing::set_st(routing::st_t::first);

  graph g(3);
  vertex src = *(vertices(g).first);
  vertex mid = *(vertices(g).first + 1);
  vertex dst = *(vertices(g).first + 2);
  edge e1 = add_edge(src, mid, g).first;
  edge e2 = add_edge(src, mid, g).first;
  edge e3 = add_edge(mid, dst, g).first;

  // Props of edge e1.
  boost::get(boost::edge_weight, g, e1) = 2;
  boost::get(boost::edge_su, g, e1) = {{0, 2}};

  // Props of edge e2.
  boost::get(boost::edge_weight, g, e2) = 1;
  boost::get(boost::edge_su, g, e2) = {{0, 1}};

  // Props of edge e3.
  boost::get(boost::edge_weight, g, e3) = 2;
  boost::get(boost::edge_su, g, e3) = {{0, 1}};

  generic_label_creator<graph, COST, CU> c(g, 1);
  generic_label<graph, COST, CU> l(0, {0, 3}, edge(), src);
  sol_type S, Q;
  dijkstra(g, S, Q, l, c, dst);
  // The tracer.
  generic_tracer<graph, cupath, sol_type, CU> t(g, 1);
  // Get the path.
  auto op = trace(S, dst, l, t);

  // We found the path.
  BOOST_CHECK(op.value() == cupath({0, 1}, {e2, e3}));

  // We remember at node mid the results for edges e1 and e2.
  BOOST_CHECK(S[mid].size() == 2);
  // We reach node mid by edge e2 at cost 1.
  BOOST_CHECK(get_cost(*S[mid].begin()) == 1);
  BOOST_CHECK(get_edge(*S[mid].begin()) == e2);
  // The second way of reaching node mid is by edge e1 at cost 2.
  BOOST_CHECK(get_cost(*(++S[mid].begin())) == 2);
  BOOST_CHECK(get_edge(*(++S[mid].begin())) == e1);
}

// Make sure that we don't remember the results for edge e2, because
// it offers a worse path then edge e1.  This tests for the existance
// of better old results.
BOOST_AUTO_TEST_CASE(dijkstra_test_6)
{
  graph g(2);
  vertex src = *(vertices(g).first);
  vertex dst = *(vertices(g).first + 1);

  edge e1 = boost::add_edge(src, dst, g).first;
  edge e2 = boost::add_edge(src, dst, g).first;

  // Props of edge e1.
  boost::get(boost::edge_weight, g, e1) = 1;
  boost::get(boost::edge_su, g, e1) = {{0, 1}};

  // Props of edge e2.  During the search, the result for this edge
  // won't be even added, because already the better result for edge
  // e1 will be in place.
  boost::get(boost::edge_weight, g, e2) = 2;
  boost::get(boost::edge_su, g, e2) = {{0, 1}};

  generic_label_creator<graph, COST, CU> c(g, 1);
  generic_label<graph, COST, CU> l(0, {0, 3}, edge(), src);
  sol_type S, Q;
  dijkstra(g, S, Q, l, c, dst);

  BOOST_CHECK(S[dst].size() == 1);
  BOOST_CHECK(get_cost(*S[dst].begin()) == 1);
  BOOST_CHECK(get_edge(*S[dst].begin()) == e1);
}

// Make sure that we don't remember the results for edge e1, because
// it offers a worse path then edge e2.  This tests the purging of the
// old results.
BOOST_AUTO_TEST_CASE(dijkstra_test_7)
{
  graph g(2);
  vertex src = *(vertices(g).first);
  vertex dst = *(vertices(g).first + 1);

  edge e1 = boost::add_edge(src, dst, g).first;
  edge e2 = boost::add_edge(src, dst, g).first;

  // Props of edge e1.
  boost::get(boost::edge_weight, g, e1) = 2;
  boost::get(boost::edge_su, g, e1) = {{0, 1}};

  // Props of edge e2.  During the Dijkstra::Search search, the result
  // for edge e1 will be removed, because edge e2 offers a better
  // result.
  boost::get(boost::edge_weight, g, e2) = 1;
  boost::get(boost::edge_su, g, e2) = {{0, 1}};

  generic_label_creator<graph, COST, CU> c(g, 1);
  generic_label<graph, COST, CU> l(0, {0, 3}, edge(), src);
  sol_type S, Q;
  dijkstra(g, S, Q, l, c, dst);

  BOOST_CHECK(S[dst].size() == 1);
  BOOST_CHECK(get_cost(*S[dst].begin()) == 1);
  BOOST_CHECK(get_edge(*S[dst].begin()) == e2);
}

// Test the has_better_or_equal function.
BOOST_AUTO_TEST_CASE(test_has_better_or_equal)
{
  using label = generic_label<graph, COST, CU>;

  graph g(1);
  vertex v = *(vertices(g).first);
  edge ne = *edges(g).second;

  sol_type S;
  label l(10, {1, 4}, ne, v);

  // The labels is empty, so there are no better labels.
  BOOST_CHECK(!has_better_or_equal(S, l));

  // The labels has only l, so still is a better label.
  S[v].insert(l);
  BOOST_CHECK(has_better_or_equal(S, l));

  // Label l1 is incomparable with l, because of incomparable SUs.
  label l1(30, {10, 11}, ne, v);
  BOOST_CHECK(!has_better_or_equal(S, l1));

  // l <= l2, because l is of lower cost and has the same SU.
  label l2(20, {1, 4}, ne, v);
  BOOST_CHECK(has_better_or_equal(S, l2));

  // l <= l3, because the SU of l3 is a proper subset of the SU of l,
  // and the cost of l is smaller than the cost of l3.
  label l3(10, {1, 3}, ne, v);
  BOOST_CHECK(has_better_or_equal(S, l3));
}

BOOST_AUTO_TEST_CASE(test_purge_worse)
{
  using label = generic_label<graph, COST, CU>;

  graph g(1);
  vertex v = *(vertices(g).first);
  edge ne = *edges(g).second;

  label l1(10, {1, 4}, ne, v);
  label l2(10, {1, 3}, ne, v);
  label l3(20, {1, 4}, ne, v);
  label l4(30, {1, 5}, ne, v);
  sol_type S;
  S[v].insert(l2);
  S[v].insert(l3);
  S[v].insert(l4);

  // This should remove both l2 and l3, but not l4.
  purge_worse(S, l1);
  BOOST_CHECK(S[v].count(l2) == 0);
  BOOST_CHECK(S[v].count(l3) == 0);
  BOOST_CHECK(S[v].count(l4) == 1);
}

BOOST_AUTO_TEST_CASE(test_move_label)
{
  using label = generic_label<graph, COST, CU>;

  graph g(2);
  vertex v1 = *(boost::vertices(g).first);
  vertex v2 = *(boost::vertices(g).first + 1);
  edge ne = *edges(g).second;

  label l1(10, {1, 4}, ne, v1);
  label l2(10, {1, 3}, ne, v2);
  label l3(20, {1, 4}, ne, v1);
  label l4(30, {1, 5}, ne, v2);

  sol_type S, Q;

  Q.push(l1);
  BOOST_CHECK(move_label(S, Q) == l1);
  BOOST_CHECK(Q.empty());
  BOOST_CHECK(S.size() == 1);
  BOOST_CHECK(S[v1].size() == 1);
  BOOST_CHECK(*S[v1].begin() == l1);

  Q.push(l2);
  Q.push(l3);
  BOOST_CHECK(move_label(S, Q) == l2);
  BOOST_CHECK(Q.size() == 1);
  BOOST_CHECK(*Q[v1].begin() == l3);
  BOOST_CHECK(S.size() == 2);
  BOOST_CHECK(*S[v1].begin() == l1);
  BOOST_CHECK(*S[v2].begin() == l2);

  Q.push(l4);
  BOOST_CHECK(move_label(S, Q) == l3);
  BOOST_CHECK(Q.size() == 1);
  BOOST_CHECK(*Q[v2].begin() == l4);
  BOOST_CHECK(S.size() == 2);
  BOOST_CHECK(S[v1].size() == 2);
  BOOST_CHECK(*S[v1].begin() == l1);
  BOOST_CHECK(*(++(S[v1].begin())) == l3);
  BOOST_CHECK(S[v2].size() == 1);
  BOOST_CHECK(*S[v2].begin() == l2);

  BOOST_CHECK(move_label(S, Q) == l4);
  BOOST_CHECK(Q.empty());
  BOOST_CHECK(S.size() == 2);
  BOOST_CHECK(S[v1].size() == 2);
  BOOST_CHECK(*S[v1].begin() == l1);
  BOOST_CHECK(*(++(S[v1].begin())) == l3);
  BOOST_CHECK(S[v2].size() == 2);
  BOOST_CHECK(*S[v2].begin() == l2);
  BOOST_CHECK(*(++(S[v2].begin())) == l4);
}
