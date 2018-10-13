#define BOOST_TEST_MODULE Utils

#include "cunits.hpp"
#include "sunits.hpp"
#include "utils.hpp"

#include "sample_graphs.hpp"

#include <boost/test/unit_test.hpp>

#include <random>
#include <set>
#include <tuple>
#include <vector>

using namespace std;

BOOST_AUTO_TEST_CASE(get_random_int_test)
{
  std::default_random_engine eng;
  for(int i = 0; i < 100; ++i)
    for(int j = 0; j < 10; ++j)
      {
	int n = get_random_int(0, j, eng);
	BOOST_CHECK(0 <= n && n <= j);
      }
}

BOOST_AUTO_TEST_CASE(find_path_su_test)
{
  graph g;
  vector<vertex> vs;
  vector<edge> es;
  sample_graph1(g, vs, es);

  // The su of an empty path.
  SU su1 = find_path_su(g, path());
  BOOST_CHECK(su1.empty());

  // The su of es[0].
  SU su2 = find_path_su(g, path{es[0]});
  BOOST_CHECK((su2 == SU{{0, 2}}));

  // The su of {es[0], es[1]}.
  SU su3 = find_path_su(g, path{es[0], es[1]});
  BOOST_CHECK((su3 == SU{{1, 2}}));
}
