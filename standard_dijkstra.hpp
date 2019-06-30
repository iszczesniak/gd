#ifndef STANDARD_DIJKSTRA_HPP
#define STANDARD_DIJKSTRA_HPP

#include "standard_label.hpp"
#include "standard_permanent.hpp"
#include "standard_tentative.hpp"

#include <cassert>
#include <map>

// Move the best label from T to P, and return a reference to the
// label in the new place.
template <typename Graph, typename Cost>
auto
move_label(standard_tentative<Graph, Cost> &T,
           standard_permanent<Graph, Cost> &P)
{
  return P.push(T.pop());
}

// Here we compare only the cost of the labels.  Each node can have
// only one label.
template <typename Container, typename Graph, typename Cost>
bool
has_better_or_equal(const Container &C,
                    const standard_label<Graph, Cost> &j)
{
  // The wrapped label.
  const auto &wi = C[get_target(j)];

  if (wi)
    {
      const auto &i = *wi;
      // Make sure the targets are the same.
      assert(get_target(i) == get_target(j));
      // Here we use the <= operator we define.
      return (i <= j);
    }

  return false;
}

template <typename Graph, typename Cost>
void
purge_worse(standard_tentative<Graph, Cost> &T,
            const standard_label<Graph, Cost> &j)
{
  // The tentative label for the target vertex of j.
  auto &ip = T[get_target(j)];
  // There must be no tentative label, and if there is one, it cannot
  // be better or equal to j.
  assert(!ip || !(*ip <= j));
  // And so we get here only to release the tentative label.  It's OK
  // to reset ip even if it's a nullptr.
  ip.reset();
}


#endif // STANDARD_DIJKSTRA_HPP
