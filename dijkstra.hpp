#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

#include <boost/range.hpp>

#include <list>
#include <iterator>
#include <optional>

/**
 * In graph g, find the shortest path to t.
 */
template <typename Graph, typename Permanent, typename Tentative,
          typename Label, typename Functor>
void
dijkstra(const Graph &g, Permanent &P, Tentative &T, const Label &sl,
         const Functor &f, Vertex<Graph> t)
{
  // Boot the search.
  T.push(sl);

  while(!T.empty())
    {
      const auto &l = move_label(T, P);
      const auto &v = get_target(l);

      // Stop searching when we reach the destination node.
      if (v == t)
        break;

      // Itereate over the out edges of vertex v.
      for(const auto &e: boost::make_iterator_range(out_edges(v, g)))
        relax(g, P, T, e, l, f);
    }
}

/**
 * Try to relax edge e, given label l.
 */
template <typename Graph, typename Permanent, typename Tentative,
          typename Label, typename Functor>
void
relax(const Graph &g, Permanent &P, Tentative &T, const Edge<Graph> &e,
      const Label &l, const Functor &f)
{
  try
    {
      // Candidate labels.
      auto cls = f(e, l);

      for (auto &cl: cls)
	if (!has_better_or_equal(P, cl) && !has_better_or_equal(T, cl))
	  {
	    purge_worse(T, cl);
	    // We push the new label after purging, so that purging
	    // has less work, i.e., a smaller Q.  Furthermore, in
	    // purge_worse we are using the <= operator, which would
	    // remove the label we push below.
	    T.push(std::move(cl));
	  }
    } catch (bool no_label)
    {
      assert(no_label);
    }
}

/**
 * Build the path by tracing labels.
 */
template <typename Permanent, typename Vertex, typename Label,
          typename Tracer>
std::optional<typename Tracer::path_t>
trace(const Permanent &P, Vertex dst, const Label &sl, Tracer &t)
{
  // Make sure there is the solution for vertex dst.
  if (const auto &vd = P[dst]; !std::empty(vd))
    {
      // This is the path we're building.
      typename Tracer::path_t result;

      // Get the initial label, i.e. the label for the destination.
      for(auto i = t.init(result, vd); *i != sl; i = t.advance(P, i))
        t.push(result, i);

      // Move the result to the optional object we return.
      return std::move(result);
    }

  // We return an empty optional, becase no path was found.
  return std::optional<typename Tracer::path_t>();
}

#endif // DIJKSTRA_HPP
