#include "adaptive_units.hpp"
#include "graph.hpp"
#include "utils.hpp"

#include <iostream>
#include <limits>

#define BOOST_TEST_MODULE adaptive_units
#include <boost/test/unit_test.hpp>

using namespace std;

// Test the reach.
BOOST_AUTO_TEST_CASE(adaptive_units_units)
{
  using au = adaptive_units<COST>;

  au::longest_shortest_path(6666.66667);
  BOOST_CHECK(au::units(10, 0) == 10);

  BOOST_CHECK(au::units(10, 1250) == 10);
  BOOST_CHECK(au::units(10, 1251) == 11);

  BOOST_CHECK(au::units(10, 2500) == 20);
  BOOST_CHECK(au::units(10, 2501) == 21);

  BOOST_CHECK(au::units(10, 5000) == 30);
  BOOST_CHECK(au::units(10, 5001) == 31);

  BOOST_CHECK(au::units(10, 10000) == 40);
  BOOST_CHECK(au::units(10, 10001) == std::numeric_limits<int>::max());
}

// Test the reach.
BOOST_AUTO_TEST_CASE(adaptive_units_reach)
{
  using au = adaptive_units<COST>;

  au::longest_shortest_path(6666.66667);
  BOOST_CHECK_CLOSE(au::reach(10, 10), 1250, 0.1);
  BOOST_CHECK_CLOSE(au::reach(10, 20), 2500, 0.1);
  BOOST_CHECK_CLOSE(au::reach(10, 30), 5000, 0.1);
  BOOST_CHECK_CLOSE(au::reach(10, 40), 10000, 0.1);
}

// Test the ncus.
BOOST_AUTO_TEST_CASE(adaptive_units_ncus)
{
  using au = adaptive_units<COST>;

  auto s = au::ncus(10);
  BOOST_CHECK(s.size() == 31);

  int i = 10;
  for(int units: s)
    {
      BOOST_CHECK(units == i);
      ++i;
    }
}

