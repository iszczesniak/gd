#ifndef STANDARD_TRACER_HPP
#define STANDARD_TRACER_HPP

#include "graph.hpp"

template <typename Label>
auto
get_cost(const Label &);

template <typename Label>
auto
get_edge(const Label &);

template <typename Graph, typename Permanent, typename Path>
struct standard_tracer
{
  // That's the label type we're using.
  using label_t = typename Permanent::label_t;
  // The type of vertex data.
  using vd_t = typename Permanent::vd_t;
  // The path type.  The trace function needs it.
  using path_t = Path;

  // The graph.
  const Graph &m_g;

  standard_tracer(const Graph &g): m_g(g)
  {
  }

  /**
   * Initialize the tracing.  We don't need the path here.
   */
  const label_t *
  init(path_t &, const vd_t &l)
  {
    assert(l);
    return &*l;
  }

  /**
   * Push the label to the path.
   */
  void
  push(path_t &p, const label_t *lp)
  {
    // This is the label we process.
    const auto &l = *lp;
    // The edge of the label.
    const auto &e = get_edge(l);
    // Add the label's edge to the path.
    p.push_front(e);
  }

  /**
   * We are given the iterator to the label that we process here.  We
   * require this is not the starting label.  The objective is to find
   * the previous label on the path.
   */
  const label_t *
  advance(const Permanent &P, const label_t *l_ptr)
  {
    // This is the label we process.
    const auto &l = *l_ptr;
    // This is the edge of the label.
    const auto &e = get_edge(l);
    // The source of the edge.
    const auto &s = boost::source(e, m_g);

    // Find the previous element for vertex s.
    const auto &pe = P[s];
    // The element must have a label.
    assert(pe);
    // Get the label of the element.
    const auto &pl = *pe;

    // The cost of the label.
    const auto &c = get_cost(l);
    // The edge cost.
    const auto &ec = boost::get(boost::edge_weight, m_g, e);
    // Make sure we've got the costs right.
    assert(get_cost(pl) + ec == c);

    // Return the address of the previous label.
    return &pl;
  }
};

#endif // STANDARD_TRACER_HPP
