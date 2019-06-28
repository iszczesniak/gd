#ifndef ROUTING_HPP
#define ROUTING_HPP

#include "graph.hpp"

#include <optional>

class routing
{  
public:
  // The type of specturm selection:
  // first - the first (lowest number of a unit) contiguous fragment
  // fittest - the fittest (smallest) contiguous fragment
  // random - any contiguous fragment
  enum class st_t {none, first, fittest, random};

  // The type of shortest path algorthm:
  // dijkstra - generic dijkstra
  // parallel - the search in parallel graphs
  // brtforce - the brute force
  // puyenksp - the pretty usual Yen KSP
  enum class rt_t {dijkstra, parallel, brtforce, puyenksp};

  // Try to set up the demand, i.e., find the path, and allocate
  // resources.  The result returned is the supath set up.
  static std::optional<cupath>
  set_up(graph &g, const demand &d);

  // Try to set up the demand with the given SU, i.e., find the path,
  // and allocate resources.  The result returned is the cupath set
  // up.
  static std::optional<cupath>
  set_up(graph &g, const demand &d, const CU &cu);

  // Search for a path using a given algorithm.  If function fails, no
  // result is returned.
  static std::optional<cupath>
  search(graph &g, const demand &d, const CU &cu, rt_t rt);

  // Tear down the path in the graph.  This process puts back the
  // units on the edges that are used by the path.
  static void
  tear_down(graph &g, const cupath &p);

  // The maximum length of a path.
  static void
  set_ml(std::optional<COST> ml);

  static std::optional<COST>
  get_ml();

  // The K for the k-shortest paths.
  static void
  set_K(std::optional<unsigned> K);

  static std::optional<unsigned>
  get_K();

  // Set the spectrum selection type.
  static void
  set_st(const st_t st);

  // Set the spectrum selection type.
  static void
  set_st(const std::string &st);

  // Get the spectrum selection type.
  static st_t
  get_st();

  // What another routing algorithms to run.
  static void add_another_algorithm(const rt_t rt);

  // Return the string of the routing type.
  static std::string
  to_string(routing::rt_t rt);

  // From the given SU, select the CU according to the selection
  // policy of the class.  The returned CU has exactly ncu units, even
  // though the chosen contiguous fragment from SU could have had more
  // units.
  static CU
  select_cu(const SU &, int ncu);

  static CU
  select_cu(const CU &, int ncu);

protected:
  // Set up the given path.
  static bool
  set_up_path(graph &g, const cupath &p);

  // Try to find a shortest path using the generic Dijkstra algorithm.
  static std::tuple<int, int, int, std::optional<cupath> >
  search_dijkstra(const graph &, const demand &, const CU &);

  // Try to find a shortest path in multiple graphs.  Each graph the
  // edges filtered to those only that can support the given demand.
  static std::tuple<int, int, int, std::optional<cupath> >
  search_parallel(const graph &, const demand &, const CU &);

  // Try to find a shortest path by brute force: iterate the paths.
  static std::tuple<int, int, int, std::optional<cupath> >
  search_brtforce(const graph &, const demand &, const CU &);

  // Try to find a shortest path using the Yen algorithm.  If m_K is
  // set, get at most m_K paths.
  static std::tuple<int, int, int, std::optional<cupath> >
  search_puyenksp(const graph &, const demand &, const CU &);

  // Select a CU with the lowest unit numbers from SU.  It returns the
  // whole CU, i.e. it can have more units than ncu.
  static CU
  select_first(const CU &, int ncu);

  // Select the fittest CU, i.e. the smallest CU from SU that fits the
  // required number of units.  It returns the whole available CU,
  // i.e. it can have more units than ncu.
  static CU
  select_fittest(const CU &, int ncu);

  // Select the random units, i.e. the random CU from SU that fits the
  // required number of units.  It returns the whole available CU,
  // i.e. it can have more units than ncu.
  static CU
  select_random(const CU &, int ncu);

  // Interpret the string and return the spectrum selection type.
  static st_t
  st_interpret (const std::string &st);

  // The spectrum selection type.
  static st_t m_st;

  // What another routing algorithms to use.
  static std::set<rt_t> m_aras;

  // The maximal length of a path.
  static std::optional<COST> m_ml;

  // The K for the k-shortest paths.
  static std::optional<unsigned> m_K;
};

#endif /* ROUTING_HPP */
