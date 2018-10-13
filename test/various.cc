#include "graph.hpp"
#include "custom_dijkstra_call.hpp"

#define BOOST_TEST_MODULE various
#include <boost/test/unit_test.hpp>

BOOST_AUTO_TEST_CASE(test_custom_dijkstra_call)
{
  graph g(1);
  vertex v = *(boost::vertices(g).first);

  auto ore = boost::custom_dijkstra_call(g, v, v,
                                         get(boost::edge_weight_t(), g),
                                         get(boost::vertex_index_t(), g));

  BOOST_CHECK(ore);
}

// We need to have a fake test, otherwise we crash due to a Boost bug.
BOOST_AUTO_TEST_CASE(fake)
{
}
