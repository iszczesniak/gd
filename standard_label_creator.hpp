#ifndef STANDARD_LABEL_CREATOR_HPP
#define STANDARD_LABEL_CREATOR_HPP

#include "standard_label.hpp"

#include <array>
#include <tuple>

// Creates new labels.
template <typename Graph, typename Cost>
struct standard_label_creator
{
  using Label = standard_label<Graph, Cost>;

  const Graph &m_g;

  standard_label_creator(const Graph &g):
    m_g(g)
  {
  }

  // Create a new label.  We are at the target node of label l, and we
  // take edge e.
  auto
  operator()(const Edge<Graph> &e, const Label &l) const
  {
    // The cost of the new label = cost of label l + cost of edge e.
    Cost c = get_cost(l) + boost::get(boost::edge_weight, m_g, e);

    // At cost c, and along edge e, we reach the target of e.
    return std::array<Label, 1>({Label(c, e, target(e, m_g))});
  }
};

#endif // STANDARD_LABEL_CREATOR_HPP
