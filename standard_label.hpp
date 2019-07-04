#ifndef STANDARD_LABEL_HPP
#define STANDARD_LABEL_HPP

#include <ostream>
#include <tuple>

template <typename Graph>
using Vertex = typename Graph::vertex_descriptor;

template <typename Graph>
using Edge = typename Graph::edge_descriptor;

// The label is a tuple (c, e, v) of cost c, edge descriptor e, and
// vertex descriptor v.  The label stores cost c of using edge e to
// get to vertex v.
//
// In the tuple we store an edge descriptor, not a vertex descriptor,
// so that we can allow for multigraphs (i.e. graphs with parallel
// edges).
//
// For a label we need to tell the target vertex v, which we could
// figure out from the edge, but for that we would have to store a
// reference to the graph.  It's faster to store just the vertex.

template <typename Graph, typename Cost>
struct standard_label
{
  Cost m_c;
  Edge<Graph> m_e;
  Vertex<Graph> m_t;

  standard_label(const Cost &c, const Edge<Graph> &e,
                 const Vertex<Graph> &t):
    m_c(c), m_e(e), m_t(t)
  {
  }

  bool
  operator==(const standard_label &j) const
  {
    return std::tie(m_c, m_t, m_e) ==
      std::tie(j.m_c, j.m_t, j.m_e);
  }

  bool
  operator!=(const standard_label &j) const
  {
    return std::tie(m_c, m_t, m_e) !=
      std::tie(j.m_c, j.m_t, j.m_e);
  }

  // This operator we use to determine whether this label is better or
  // equal to label j.
  bool
  operator<=(const standard_label &j) const
  {
    return m_c <= j.m_c;
  }
};

template <typename Graph, typename Cost>
std::ostream &
operator << (std::ostream &os,
             const standard_label<Graph, Cost> &l)
{
  os << "standard_label(" << l.m_c
     << ", " << l.m_e << ", " << l.m_t << ')';
  return os;
}

template <typename Graph, typename Cost>
auto
get_cost(const standard_label<Graph, Cost> &l)
{
  return l.m_c;
}

template <typename Graph, typename Cost>
auto
get_edge(const standard_label<Graph, Cost> &l)
{
  return l.m_e;
}

template <typename Graph, typename Cost>
auto
get_target(const standard_label<Graph, Cost> &l)
{
  return l.m_t;
}

#endif // STANDARD_LABEL_HPP
