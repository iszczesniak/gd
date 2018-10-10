#ifndef STANDARD_CONSTRAINED_LABEL_CREATOR_HPP
#define STANDARD_CONSTRAINED_LABEL_CREATOR_HPP

#include "standard_label.hpp"

#include <array>
#include <tuple>

// Creates new labels.
template <typename Graph, typename Cost>
struct standard_constrained_label_creator
{
  using Label = standard_label<Graph, Cost>;

  const Graph &m_g;

  const Cost m_max_cost;

  standard_constrained_label_creator(const Graph &g, Cost max_cost):
    m_g(g), m_max_cost(max_cost)
  {
  }

  // Create a new label.  We are at the target node of label l, and we
  // try to take edge e.
  auto
  operator()(const Edge<Graph> &e, const Label &l) const
  {
    // The cost of the new label = cost of label l + cost of edge e.
    Cost c = get_cost(l) + boost::get(boost::edge_weight, m_g, e);

    if (c <= m_max_cost)
      // At cost c, and along edge e, we reach the target of e.
      return std::array<Label, 1>({Label(c, e, target(e, m_g))});

    // If we went over the reach, throw true.
    throw true;
  }
};

#endif // STANDARD_CONSTRAINED_LABEL_CREATOR_HPP
