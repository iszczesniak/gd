#ifndef GENERIC_CONSTRAINED_LABEL_CREATOR_HPP
#define GENERIC_CONSTRAINED_LABEL_CREATOR_HPP

#include "adaptive_units.hpp"
#include "generic_label_creator.hpp"
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
class generic_constrained_label_creator:
  generic_label_creator<Graph, Cost, Units>
{
  using base = generic_label_creator<Graph, Cost, Units>;
  using Label = generic_label<Graph, Cost, Units>;

  // The number of contiguous units initially requested.
  const int m_ncu;

public:
  generic_constrained_label_creator(const Graph &g, int ncu):
    base(g), m_ncu(ncu)
  {
  }

  std::list<Label>
  operator()(const Edge<Graph> &e, const Label &l) const
  {
    auto f = [&m_ncu = m_ncu](Cost c, SU &su)
             {
               su.remove(adaptive_units<Cost>::units(m_ncu, c));
             };

    return base::operator()(e, l, f);
  }
};

#endif // GENERIC_CONSTRAINED_LABEL_CREATOR_HPP
