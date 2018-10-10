#ifndef STANDARD_LABEL_HPP
#define STANDARD_LABEL_HPP

#include <ostream>
#include <tuple>

template <typename Graph>
using Vertex = typename Graph::vertex_descriptor;

template <typename Graph>
using Edge = typename Graph::edge_descriptor;

// The label is a tuple (c, e, v) of cost c, edge descriptor e, and
// vertex descriptor v.  The label stores cost c of using edge e.
//
// In the tuple we store an edge descriptor, not a vertex descriptor,
// so that we can allow for multigraphs (i.e. graphs with parallel
// edges).
//
// Even though we could figure out the target vertex from the edge, we
// have to store it in the tuple to make a tuple unique, because in
// Boost the descriptors of a directed edge and its reverse are the
// same.

template <typename Graph, typename Cost>
struct standard_label: std::tuple<Cost, Edge<Graph>, Vertex<Graph>>
{
  using self = standard_label<Graph, Cost>;
  using base = std::tuple<Cost, Edge<Graph>, Vertex<Graph>>;

  standard_label()
  {
  }

  standard_label(const Cost &c, const Edge<Graph> &e,
                 const Vertex<Graph> &t):
    base(c, e, t)
  {
  }

  bool
  operator!=(const self &l) const
  {
    return static_cast<base>(*this) != static_cast<base>(l);
  }

  bool
  operator<(const self &l) const
  {
    return static_cast<base>(*this) < static_cast<base>(l);
  }

  std::ostream &
  operator << (std::ostream &os)
  {
    const self &l = *this;
    os << "standard_label(" << std::get<0>(l) << ", "
       << std::get<1>(l) << ", " << std::get<2>(l) << ')';
    return os;
  }
};

template <typename Graph, typename Cost>
auto
get_cost(const standard_label<Graph, Cost> &l)
{
  return std::get<0>(l);
}

template <typename Graph, typename Cost>
auto
get_edge(const standard_label<Graph, Cost> &l)
{
  return std::get<1>(l);
}

template <typename Graph, typename Cost>
auto
get_target(const standard_label<Graph, Cost> &l)
{
  return std::get<2>(l);
}

#endif // STANDARD_LABEL_HPP
