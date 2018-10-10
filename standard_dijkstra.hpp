#ifndef STANDARD_DIJKSTRA_HPP
#define STANDARD_DIJKSTRA_HPP

#include "standard_label.hpp"
#include "standard_solution.hpp"

#include <cassert>
#include <map>

// Move the best label from Q to L, and return a reference to the
// label in the new place.
template <typename Graph, typename Cost>
auto
move_label(standard_solution<Graph, Cost> &S,
           standard_solution<Graph, Cost> &Q)
{
  auto p = S.insert(Q.pop());

  // Make sure we moved the label.
  assert(p.inserted);

  // That's the label.
  const auto &l = p.position->second;

  return l;
}

// Here we compare only the cost of the labels.  Each node can have
// only one label.
template <typename Graph, typename Cost>
bool
has_better_or_equal(const standard_solution<Graph, Cost> &S,
                    const standard_label<Graph, Cost> &j)
{
  // Find the target node of label l.
  if (auto iter = S.find(get_target(j)); iter != S.end())
    {
      // OK, there is a label for the target node of label j.
      const auto &i = iter->second;
      // Make sure the targets are the same.
      assert(get_target(i) == get_target(j));
      // True, if the cost of label i <= the cost of label j.
      return get_cost(i) <= get_cost(j);
    }

  return false;
}

template <typename Graph, typename Cost>
void
purge_worse(standard_solution<Graph, Cost> &Q,
            const standard_label<Graph, Cost> &j)
{
  // Do nothing, because even if there is a worse label (i.e., of the
  // higher cost), it will be overwritten with a better label in the
  // relax function.

  // Just make sure there is no equal label, because we do not push
  // equal labels into the priority queue Q.
  assert(Q.find(get_target(j)) == Q.end() ||
         get_cost(Q.find(get_target(j))->second) != get_cost(j));
}

#endif // STANDARD_DIJKSTRA_HPP
