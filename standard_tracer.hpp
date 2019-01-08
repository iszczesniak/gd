#ifndef STANDARD_TRACER_HPP
#define STANDARD_TRACER_HPP

#include "graph.hpp"

template <typename Label>
auto
get_cost(const Label &);

template <typename Label>
auto
get_edge(const Label &);

template <typename Graph, typename Path, typename Solution>
class standard_tracer
{
  using Label = typename Solution::mapped_type;

  // The graph.
  const Graph &m_g;

public:
  using path_type = Path;

  standard_tracer(const Graph &g): m_g(g)
  {
  }

  /**
   * Initialize the tracing.
   */
  const Label *
  init(Path &p, typename Solution::const_iterator i)
  {
    return &i->second;
  }

  /**
   * Push the label to the path.
   */
  void
  push(Path &p, const Label *lp)
  {
    // This is the label we process.
    const Label &l = *lp;
    // The edge of the label.
    const Edge<Graph> &e = get_edge(l);
    // Add the label's edge to the path.
    p.push_front(e);
  }

  /**
   * We are given the iterator to the label that we process here.  We
   * require this is not the starting label.  The objective is to find
   * the previous label on the path.
   */
  const Label *
  advance(const Solution &S, const Label *lp)
  {
    // This is the label we process.
    const Label &l = *lp;
    // This is the edge of the label.
    const Edge<Graph> &e = get_edge(l);
    // The edge source.
    auto s = boost::source(e, m_g);

    // Find the label at node s.
    auto i = S.find(s);
    assert(i != S.end());

    // The cost of the label.
    const auto &c = get_cost(l);
    // The edge cost.
    auto ec = boost::get(boost::edge_weight, m_g, e);
    // This is the previous label.
    const Label &pl = i->second;
    assert(get_cost(pl) + ec == c);

    return &i->second;
  }
};

#endif // STANDARD_TRACER_HPP
