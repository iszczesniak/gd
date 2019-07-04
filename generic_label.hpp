#ifndef GENERIC_LABEL_HPP
#define GENERIC_LABEL_HPP

#include <ostream>
#include <set>
#include <tuple>

template <typename Graph>
using Vertex = typename Graph::vertex_descriptor;

template <typename Graph>
using Edge = typename Graph::edge_descriptor;

// The label is a tuple (c, S, e, v) of cost c, SU S, edge descriptor
// e, and vertex descriptor v.  The label stores cost c and SU S of
// using edge e to get to vertex v.
//
// In the tuple we store an edge descriptor, not a vertex descriptor,
// so that we can allow for multigraphs (i.e. graphs with parallel
// edges).
//
// For a label we need to tell the target vertex v, which we could
// figure out from the edge, but for that we would have to store a
// reference to the graph.  It's faster to store just the vertex.

template <typename Graph, typename Cost, typename Units>
struct generic_label
{
  Cost m_c;
  Units m_u;
  Edge<Graph> m_e;
  Vertex<Graph> m_t;

  generic_label(const Cost &c, const Units &u, const Edge<Graph> &e,
                const Vertex<Graph> &t):
    m_c(c), m_u(u), m_e(e), m_t(t)
  {
  }

  generic_label(const Cost &c, Units &&u, const Edge<Graph> &e,
                const Vertex<Graph> &t):
    m_c(c), m_u(std::move(u)), m_e(e), m_t(t)
  {
  }

  generic_label(const generic_label &) = default;

  generic_label(generic_label &&) = default;

  generic_label &
  operator = (generic_label &&) = default;

  bool
  operator == (const generic_label &j) const
  {
    return std::tie(m_c, m_t, m_e, m_u) ==
      std::tie(j.m_c, j.m_t, j.m_e, j.m_u);
  }

  bool
  operator != (const generic_label &j) const
  {
    // We compare first the cost and target, since they are the most
    // likely to differ.  We compare the units at the very end,
    // because that comparison is time-consuming.
    return std::tie(m_c, m_t, m_e, m_u) !=
      std::tie(j.m_c, j.m_t, j.m_e, j.m_u);
  }

  // This operator is used by containers to establish the order
  // between labels.  All we really care about is the cost, and the
  // units.  The order of units matters, because it influences the
  // spectrum allocation policy.  Here the units are sorted by their
  // increasing min() numbers (the < operator of cunits), and so the
  // specturm allocation policy is the first-fit.
  //
  // To distinguish different labels, we need to take into account the
  // other label properties, and to this end we use the operator < of
  // a tuple.  Tuple should be optimized out, so there is no overhead
  // in using the tuple here.
  bool
  operator < (const generic_label &j) const
  {
    return std::tie(m_c, m_u, m_e, m_t) <
      std::tie(j.m_c, j.m_u, j.m_e, j.m_t);
  }

  // This operator is used by our algorithm.
  bool
  operator <= (const generic_label &j) const
  {
    return m_c <= j.m_c && m_u.includes(j.m_u);
  }
};

template <typename Graph, typename Cost, typename Units>
std::ostream &
operator << (std::ostream &os,
             const generic_label<Graph, Cost, Units> &l)
{
  os << "generic_label(" << l.m_c << ", " << l.m_u
     << ", " << l.m_e << ", " << l.m_t << ')';
  return os;
}

template <typename Graph, typename Cost, typename Units>
auto
get_cost(const generic_label<Graph, Cost, Units> &l)
{
  return l.m_c;
}

template <typename Graph, typename Cost, typename Units>
auto
get_units(const generic_label<Graph, Cost, Units> &l)
{
  return l.m_u;
}

template <typename Graph, typename Cost, typename Units>
auto
get_edge(const generic_label<Graph, Cost, Units> &l)
{
  return l.m_e;
}

template <typename Graph, typename Cost, typename Units>
auto
get_target(const generic_label<Graph, Cost, Units> &l)
{
  return l.m_t;
}

#endif // GENERIC_LABEL_HPP
