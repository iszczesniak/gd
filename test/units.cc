#define BOOST_TEST_MODULE units

#include "units.hpp"

#include <boost/test/unit_test.hpp>

#include <algorithm>

using namespace std;

BOOST_AUTO_TEST_CASE(cunits_test)
{
  CU cu1(0, 1);
  CU cu2(3, 5);

  // Test of size.
  BOOST_CHECK(cu1.count() == 1);
  BOOST_CHECK(cu2.count() == 2);

  // Test of the operator < for cunits.
  BOOST_CHECK(cu1 < cu2);
  BOOST_CHECK(!(cu2 < cu1));

  // Test of the operator ==.
  BOOST_CHECK((cu1 == CU{0, 1}));
  BOOST_CHECK(cu2 == cu2);

  // Test of the operator !=.
  BOOST_CHECK(cu1 != cu2);
  
  // Test of the includes function.
  BOOST_CHECK(cu1.includes({0, 1}));
  BOOST_CHECK(cu2.includes({3, 4}));
  BOOST_CHECK(cu2.includes({3, 5}));
  BOOST_CHECK(cu2.includes({4, 5}));
  BOOST_CHECK(!cu1.includes({0, 2}));
  BOOST_CHECK(!cu1.includes({2, 3}));
  BOOST_CHECK(!cu2.includes({2, 4}));
  BOOST_CHECK(!cu2.includes({3, 6}));
  BOOST_CHECK(!cu2.includes({5, 6}));
}

BOOST_AUTO_TEST_CASE(sunits_insert_test1)
{
  SU su;

  su.insert({1, 2});
  BOOST_CHECK((su == SU{{1, 2}}));

  su.insert({3, 4});
  BOOST_CHECK((su == SU{{1, 2}, {3, 4}}));

  su.insert({0, 1});
  BOOST_CHECK((su == SU{{0, 2}, {3, 4}}));

  su.insert({4, 5});
  BOOST_CHECK((su == SU{{0, 2}, {3, 5}}));

  su.insert({2, 3});
  BOOST_CHECK((su == SU{{0, 5}}));
}

// It's the same test as above, but the CU sizes is 2.
BOOST_AUTO_TEST_CASE(sunits_insert_test2)
{
  SU su;

  su.insert({2, 4});
  BOOST_CHECK((su == SU{{2, 4}}));

  su.insert({6, 8});
  BOOST_CHECK((su == SU{{2, 4}, {6, 8}}));

  su.insert({0, 2});
  BOOST_CHECK((su == SU{{0, 4}, {6, 8}}));

  su.insert({8, 10});
  BOOST_CHECK((su == SU{{0, 4}, {6, 10}}));

  su.insert({4, 6});
  BOOST_CHECK((su == SU{{0, 10}}));
}

BOOST_AUTO_TEST_CASE(remove_test1)
{
  SU su{{0, 5}};

  su.remove({2, 3});
  BOOST_CHECK((su == SU{{0, 2}, {3, 5}}));

  su.remove({4, 5});
  BOOST_CHECK((su == SU{{0, 2}, {3, 4}}));

  su.remove({0, 1});
  BOOST_CHECK((su == SU{{1, 2}, {3, 4}}));

  su.remove({3, 4});
  BOOST_CHECK((su == SU{{1, 2}}));

  su.remove({1, 2});
  BOOST_CHECK(su.empty());
}

// It's the same test as above, but the CU sizes is 2.
BOOST_AUTO_TEST_CASE(remove_test2)
{
  SU su{{0, 10}};

  su.remove({4, 6});
  BOOST_CHECK((su == SU{{0, 4}, {6, 10}}));

  su.remove({8, 10});
  BOOST_CHECK((su == SU{{0, 4}, {6, 8}}));

  su.remove({0, 2});
  BOOST_CHECK((su == SU{{2, 4}, {6, 8}}));

  su.remove({6, 8});
  BOOST_CHECK((su == SU{{2, 4}}));

  su.remove({2, 4});
  BOOST_CHECK(su.empty());
}

BOOST_AUTO_TEST_CASE(remove_ncu_test)
{
  SU su;

  su.remove(2);
  BOOST_CHECK(su.empty());

  su.insert({0, 1});
  su.remove(2);
  BOOST_CHECK(su.empty());

  su.insert({0, 2});
  su.remove(2);
  BOOST_CHECK(su.size() == 1);  
  su.remove(3);
  BOOST_CHECK(su.size() == 0);
}

// Simple tests with a single CU.
BOOST_AUTO_TEST_CASE(includes_test)
{
  // Both a and b have the same CU.
  {
    SU a, b;

    // Empty set includes empty set. OK.
    BOOST_CHECK(a.includes(b));

    b.insert({10, 11});
    BOOST_CHECK(!a.includes(b));

    a.insert({10, 11});
    BOOST_CHECK(a.includes(b));
  }

  // CU of "a" only partially includes the tested sets.
  {
    SU a;
    a.insert({10, 12});

    BOOST_CHECK(!a.includes({{9, 11}}));
    BOOST_CHECK(!a.includes({{11, 13}}));
  }

  // CU of "a" properly includes the tested sets.
  {
    SU a;
    a.insert({10, 12});

    BOOST_CHECK(a.includes({{10, 11}}));
    BOOST_CHECK(a.includes({{11, 12}}));
  }

  // SU a preceeds b, they touch.  First of size 1, and then 2.
  {
    SU a, b;

    a.insert({1, 2});
    b.insert({2, 3});
    BOOST_CHECK(!a.includes(b));

    a.insert({0, 1});
    b.insert({3, 4});
    BOOST_CHECK(!a.includes(b));
  }

  // SU b preceeds a, they touch.  First of size 1, and then 2.
  {
    SU a, b;

    b.insert({1, 2});
    a.insert({2, 3});
    BOOST_CHECK(!a.includes(b));

    b.insert({0, 1});
    a.insert({3, 4});
    BOOST_CHECK(!a.includes(b));
  }

  // SU a preceeds b, they don't touch.  Size 1.
  {
    SU a, b;

    a.insert({0, 1});
    b.insert({2, 3});
    BOOST_CHECK(!a.includes(b));
  }

  // SU b preceeds a, they don't touch.  Size 1.
  {
    SU a, b;

    a.insert({2, 3});
    b.insert({0, 1});
    BOOST_CHECK(!a.includes(b));
  }
}

// These are more complicated tests.
BOOST_AUTO_TEST_CASE(includes_test2)
{
  {
    SU a = {{0, 1}, {2, 3}};
    BOOST_CHECK(!a.includes({{1, 2}}));
  }

  {
    SU a = {{0, 10}};
    BOOST_CHECK(a.includes({{0, 2}, {4, 7}, {9, 10}}));
  }

  {
    SU a = {{0, 10}, {20, 30}};
    BOOST_CHECK(a.includes(a));
  }

  {
    SU a = {{0, 10}, {20, 30}};
    BOOST_CHECK(a.includes({{0, 2}, {4, 7}, {20, 21}, {29, 30}}));
  }

  {
    SU a = {{0, 10}, {20, 30}};
    BOOST_CHECK(a.includes({{0, 10}}));
    BOOST_CHECK(a.includes({{20, 30}}));
    BOOST_CHECK(!a.includes({{0, 30}}));
  }
}

BOOST_AUTO_TEST_CASE(intersection_test)
{
  {
    SU a = {{0, 10}}, b = {{5, 15}};
    BOOST_CHECK((intersection(a, b) == SU{{5, 10}}));
    BOOST_CHECK((intersection(b, a) == SU{{5, 10}}));
  }

  {
    SU a = {{0, 10}, {20, 30}}, b = {{0, 100}};
    BOOST_CHECK((intersection(a, b) == a));
    BOOST_CHECK((intersection(b, a) == a));
  }

  {
    SU a = {{0, 2}, {3, 6}, {7, 8}};
    SU b = {{0, 1}, {2, 4}, {5, 8}};
    SU c = {{0, 1}, {3, 4}, {5, 6}, {7, 8}};
    BOOST_CHECK((intersection(a, b) == c));
    BOOST_CHECK((intersection(b, a) == c));
  }

  {
    SU a = {{0, 1}, {2, 3}};
    SU b = {{4, 5}};
    BOOST_CHECK((intersection(a, b).empty()));
    BOOST_CHECK((intersection(b, a).empty()));
  }
}

bool
in(list<CU> &l, const CU &cu)
{
  return find(l.begin(), l.end(), cu) != l.end();
}

BOOST_AUTO_TEST_CASE(get_candidate_sus_test)
{
  SU su;
  su.insert({0, 1});
  su.insert({2, 4});
  su.insert({5, 8});
  su.insert({9, 13});

  // Just a single slice.
  {
    auto s = get_candidate_slots(su, 1);
    BOOST_CHECK(s.size() == 10);
    BOOST_CHECK(in(s, {0, 1}));
    BOOST_CHECK(in(s, {2, 3}));
    BOOST_CHECK(in(s, {3, 4}));
    BOOST_CHECK(in(s, {5, 6}));
    BOOST_CHECK(in(s, {6, 7}));
    BOOST_CHECK(in(s, {7, 8}));
    BOOST_CHECK(in(s, {9, 10}));
    BOOST_CHECK(in(s, {10, 11}));
    BOOST_CHECK(in(s, {11, 12}));
    BOOST_CHECK(in(s, {12, 13}));
  }

  // Two slices.
  {
    auto s = get_candidate_slots(su, 2);
    BOOST_CHECK(s.size() == 6);
    BOOST_CHECK(in(s, {2, 4}));
    BOOST_CHECK(in(s, {5, 7}));
    BOOST_CHECK(in(s, {6, 8}));
    BOOST_CHECK(in(s, {9, 11}));
    BOOST_CHECK(in(s, {10, 12}));
    BOOST_CHECK(in(s, {11, 13}));
  }

  // Three slices.
  {
    auto s = get_candidate_slots(su, 3);
    BOOST_CHECK(s.size() == 3);
    BOOST_CHECK(in(s, {5, 8}));
    BOOST_CHECK(in(s, {9, 12}));
    BOOST_CHECK(in(s, {10, 13}));
  }

  // Three slices.
  {
    auto s = get_candidate_slots(su, 4);
    BOOST_CHECK(s.size() == 1);
    BOOST_CHECK(in(s, {9, 13}));
  }
}

BOOST_AUTO_TEST_CASE(operator_test)
{
  istringstream str1("{10, 20}");
  CU cu;
  BOOST_CHECK(str1 >> cu);
  BOOST_CHECK(cu == CU(10, 20));

  istringstream str2("{{10, 20}, {30, 40}}");
  SU su;
  BOOST_CHECK(str2 >> su);
  BOOST_CHECK((su == SU{{10, 20}, {30, 40}}));
}
