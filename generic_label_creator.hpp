#ifndef GENERAL_LABEL_CREATOR_HPP
#define GENERAL_LABEL_CREATOR_HPP

#include "adaptive_units.hpp"
#include "generic_label.hpp"
#include "graph.hpp"

#include <list>
#include <optional>

template <typename Label>
auto
get_cost(const Label &);

template <typename Label>
auto
get_units(const Label &);

template <typename Graph, typename Cost, typename Units>
class generic_label_creator
{
  using Label = generic_label<Graph, Cost, Units>;

  // The graph.
  const Graph &m_g;

  // The number of contiguous units initially requested.
  int m_ncu;

public:
  generic_label_creator(const Graph &g, int ncu):
    m_g(g), m_ncu(ncu)
  {
  }

  std::list<Label>
  operator()(const Edge<Graph> &e, const Label &l) const
  {
    // The cost of the edge.
    Cost ec = boost::get(boost::edge_weight, m_g, e);
    // Candidate cost.
    Cost c_c = get_cost(l) + ec;

    // The label units.
    const Units &l_units = get_units(l);
    // The units available on the edge.
    const auto &e_su = boost::get(boost::edge_su, m_g, e);
    // The candidate SU: the su of label l that can be carried by
    // edge e, and that has at least ncu contiguous units.
    auto c_su = intersection(SU{l_units}, e_su);
    c_su.remove(adaptive_units<COST>::units(m_ncu, c_c));

    std::list<Label> ls;

    for (auto &cu: c_su)
      // The candidate label.
      ls.push_back(Label(c_c, std::move(cu), e, target(e, m_g)));

    return ls;
  }
};

#endif // GENERAL_LABEL_CREATOR_HPP
