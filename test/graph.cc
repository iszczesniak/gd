#define BOOST_TEST_MODULE graph

#include "generic_label.hpp"
#include "graph.hpp"

#include <boost/graph/connected_components.hpp>
#include <boost/test/unit_test.hpp>
#include <set>

#include <memory>

using namespace std;

// The tests for the default-constructed edges.
BOOST_AUTO_TEST_CASE(default_edge_test)
{
  edge ne1, ne2;
  BOOST_CHECK(ne1 == ne2);
  unique_ptr<edge> nep = make_unique<edge>();
  BOOST_CHECK(ne1 == *nep);
  BOOST_CHECK(ne2 == *nep);
}

BOOST_AUTO_TEST_CASE(undirected_edge_test)
{
  graph g(2);
  vertex a = *(vertices(g).first);
  vertex b = *(vertices(g).first + 1);

  // Add a single edge using the boost add_edge function.
  boost::add_edge(a, b, g);

  // Get an edge descriptor of the normal edge.
  auto p1 = boost::edge(a, b, g);
  // Get an edge descriptor of the reverse edge.
  auto p2 = boost::edge(b, a, g);
  // Make sure we got the descriptros.
  BOOST_CHECK(p1.second && p2.second);
  // Make sure the edges are the same.
  BOOST_CHECK(p1 == p2);
  // Yet, BGL can tell what the source and target nodes are, and they
  // are different for e1 and e2.
  BOOST_CHECK(boost::source(p1.first, g) == a);
  BOOST_CHECK(boost::target(p1.first, g) == b);
  BOOST_CHECK(boost::source(p2.first, g) == b);
  BOOST_CHECK(boost::target(p2.first, g) == a);
}

BOOST_AUTO_TEST_CASE(end_edge_iterator_test)
{
  graph g(2);
  vertex a = *(vertices(g).first);
  vertex b = *(vertices(g).first + 1);

  // Does the end edge iterator change after adding an edge?
  auto p1 = edges(g);
  boost::add_edge(a, b, g);
  auto p2 = edges(g);

  BOOST_CHECK(p1.first != p2.first);
  BOOST_CHECK(p1.second == p2.second);

  // The edge descriptors should be the same.
  BOOST_CHECK(*(p1.second) == *(p2.second));
}

BOOST_AUTO_TEST_CASE(parallel_edge_test)
{
  graph g(2);
  vertex a = *(vertices(g).first);
  vertex b = *(vertices(g).first + 1);
  edge e1, e2;
  bool r1, r2;
  tie(e1, r1) = add_edge(a, b, g);
  tie(e2, r2) = add_edge(a, b, g);

  // Make sure we can represent multi-graphs.  That we successfully
  // added the edges, and that the edges are different.
  BOOST_CHECK(r1);
  BOOST_CHECK(r2);
  BOOST_CHECK(e1 != e2);
  BOOST_CHECK(!(e1 == e2));

  // Make sure that parallel edges are differently identified by a
  // map.
  std::map<edge, int> e2i;
  e2i[e1] = 1;
  e2i[e2] = 2;
  BOOST_CHECK(e2i[e1] == 1);
  BOOST_CHECK(e2i[e2] == 2);
  BOOST_CHECK(e2i.size() == 2);
}

BOOST_AUTO_TEST_CASE(connected_components_test)
{
  graph g(2);
  vertex a = *(vertices(g).first);
  vertex b = *(vertices(g).first + 1);
  edge e;
  bool r;
  tie(e, r) = add_edge(a, b, g);

  std::vector<int> c(num_vertices(g));
  // "num" is the number of connected components.
  int num = boost::connected_components(g, &c[0]);
  BOOST_CHECK(num == 1);

  tie(e, r) = add_edge(b, a, g);

  // "num" is the number of connected components.
  num = boost::connected_components(g, &c[0]);
  BOOST_CHECK(num == 1);
}

BOOST_AUTO_TEST_CASE(connected_components_test2)
{
  graph g(4);
  vertex a = *(vertices(g).first);
  vertex b = *(vertices(g).first + 1);
  vertex c = *(vertices(g).first + 2);
  vertex d = *(vertices(g).first + 3);
      
  add_edge(b, d, g);
  add_edge(c, d, g);

  std::vector<int> comp(num_vertices(g));
  // "num" is the number of connected components.
  int num = boost::connected_components(g, &comp[0]);
  BOOST_CHECK(num == 2);
}

BOOST_AUTO_TEST_CASE(default_generic_label_test)
{
  using label = generic_label<graph, COST, SU>;
  label l1, l2;
  unique_ptr<label> lp = make_unique<label>();

  BOOST_CHECK(!(l1 != l2));
  BOOST_CHECK(get_cost(l1) == get_cost(l2));
  BOOST_CHECK(get_units(l1) == get_units(l2));
  BOOST_CHECK(get_edge(l1) == get_edge(l2));
  BOOST_CHECK(get_target(l1) == get_target(l2));

  BOOST_CHECK(!(l1 != *lp));
  BOOST_CHECK(get_cost(l1) == get_cost(*lp));
  BOOST_CHECK(get_units(l1) == get_units(*lp));
  BOOST_CHECK(get_edge(l1) == get_edge(*lp));
  BOOST_CHECK(get_target(l1) == get_target(*lp));
}

// The test for "labels".  We want to make sure that:
//
// * no two equal "label" objects are in "labels", because "labels" is
//   a set,
//
// * we test (though no longer rely on this in the code), that an edge
//   descriptor and the descriptor of its reverse edge are equal,
//
// * a label with a default-constructed edge can be stored in
//   "labels".
BOOST_AUTO_TEST_CASE(generic_labels_test)
{
  using label = generic_label<graph, COST, SU>;
  using labels = generic_labels<graph, COST, SU>;

  graph g(2);
  vertex a = *(vertices(g).first);
  vertex b = *(vertices(g).first + 1);

  // Add a single edge using the boost add_edge function.
  boost::add_edge(a, b, g);
  // Get the normal and reverse edge descriptors.
  auto ne = boost::edge(a, b, g);
  auto re = boost::edge(b, a, g);
  BOOST_CHECK(ne == re);

  labels ls;

  // These labels are equal.
  label l0a;
  label l0b(0, {}, edge(), vertex());

  // These labels are equal.
  label l1a(0, {}, ne.first, graph::null_vertex());
  label l1b(0, {}, re.first, graph::null_vertex());

  // These labels are equal.
  label l2a(1, {{1, 2}}, ne.first, graph::null_vertex());
  label l2b(1, {{1, 2}}, re.first, graph::null_vertex());

  // These items should be stored in "labels".
  ls.insert(l0a);
  ls.insert(l0b);
  ls.insert(l1a);
  ls.insert(l1b);
  ls.insert(l2a);
  ls.insert(l2b);

  // Try to insert the labels again.
  ls.insert(l0a);
  ls.insert(l0b);
  ls.insert(l1a);
  ls.insert(l1b);
  ls.insert(l2a);
  ls.insert(l2b);

  BOOST_CHECK(ls.size() == 3);
  BOOST_CHECK(ls.count(l0a) == 1);
  BOOST_CHECK(ls.count(l0b) == 1);
  BOOST_CHECK(ls.count(l1a) == 1);
  BOOST_CHECK(ls.count(l1b) == 1);
  BOOST_CHECK(ls.count(l2a) == 1);
  BOOST_CHECK(ls.count(l2b) == 1);
}
