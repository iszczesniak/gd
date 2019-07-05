#ifndef GENERIC_DIJKSTRA_HPP
#define GENERIC_DIJKSTRA_HPP

#include "dijkstra.hpp"
#include "generic_permanent.hpp"
#include "generic_tentative.hpp"

#include <utility>

// Move the best label from T to P, and return a reference to the
// label in the new place.
template <typename Graph, typename Cost, typename Units>
auto
move_label(generic_tentative<Graph, Cost, Units> &T,
           generic_permanent<Graph, Cost, Units> &P)
{
  return P.push(T.pop());
}

/**
 * Is there in P a label that is better than or equal to label j?
 * Here we iterate over the labels in P from the beginning, because
 * there are the labels, which are the most likely to be better or
 * equal to label j, so in this way we return the fastest.
 */
template <typename Graph, typename Cost, typename Units>
bool
has_better_or_equal(const generic_permanent<Graph, Cost, Units> &P,
		    const generic_label<Graph, Cost, Units> &j)
{
  // We could go for the easy implementation where we iterate for each
  // label i, and compare it to label j.  But we take advantage of the
  // fact that the elements in the vector are sorted by cost first.
  for (const auto &i: P[get_target(j)])
    {
      // Stop searching when we reach a label with a higher cost.  If
      // the cost of label i is higher than the cost of label j, then
      // label i (and the labels in the vector that follow) cannot be
      // better or equal (they can be incomparable or worse).
      if (get_cost(i) > get_cost(j))
        break;

      // Is label i better than or equal to label j?
      if (i <= j)
        return true;
    }

  return false;
}

/**
 * Is there in T a label that is better than or equal to label j?
 */
template <typename Graph, typename Cost, typename Units>
bool
has_better_or_equal(const generic_tentative<Graph, Cost, Units> &T,
		    const generic_label<Graph, Cost, Units> &j)
{
  // We could go for the easy implementation where we iterate for each
  // label i, and compare it to label j.  But we take advantage of the
  // fact that the elements in the set are sorted by cost first.

  // Iterate over all tentative labels.
  for (const auto &i: T[get_target(j)])
    {
      // Stop searching when we reach a label with a higher cost.  If
      // the cost of label i is higher than the cost of label j, then
      // label i (and the labels in the vector that follow) cannot be
      // better or equal (they can be incomparable or worse).
      if (get_cost(i) > get_cost(j))
        break;

      // Is label i better than or equal to label j?
      if (i <= j)
        return true;
    }

  return false;
}

/**
 * Purge from queue Q those labels which are worse than label j.
 */
template <typename Graph, typename Cost, typename Units>
void
purge_worse(generic_tentative<Graph, Cost, Units> &T,
	    const generic_label<Graph, Cost, Units> &j)
{
  auto &Tt = T[get_target(j)];

  // We could go for the easy implementation where we iterate for each
  // label i and compare it to j.  But we take advantage of the fact
  // that the elements in the set are sorted by cost first.  We
  // iterate in the reverse order!
  for(auto r = Tt.rbegin(); r != Tt.rend();)
    {
      const auto &i = *r;

      // Stop searching when we reach label i with the cost lower than
      // the cost of label j.  If the cost of label i is lower than
      // the cost of label j, then label i (and the labels in the set
      // that follow) cannot be worse (they can be better or
      // incomparable).
      if (get_cost(i) < get_cost(j))
        break;

      // Make sure labels i and j are not equal.  We can make this
      // assertion here, because we are not inserting equal labels
      // into the priority queue.  We need this assertion here, so
      // that we can sefely use the <= operator below.
      assert(!(get_cost(i) == get_cost(j) &&
               get_units(i) == get_units(j)));

      // To check whether label i is worse then j, we use the <=
      // operator, because we made sure the labels are not equal.
      if (j <= i)
        // We want to remove label i, and we're going to use iterator
        // r.  We can safely remove the element pointed to by r,
        // because the base iterator points to the element next to r.
        // This erasure does not invalidate the base iterator of r.
        // Note that we do not increment r, because after the erasure,
        // r will already point to the next element.
        Tt.erase(--(r.base()));
      else
        ++r;
    }
}

#endif // GENERIC_DIJKSTRA_HPP
