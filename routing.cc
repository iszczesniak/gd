#include "routing.hpp"

// I need to include this file here, because it introduces the
// specializations of the "target" and "get" functions, which have to
// be known before the standard_dijkstra.hpp is included.
#include <boost/graph/filtered_graph.hpp>

#include "accountant.hpp"
#include "accounted_solution.hpp"
#include "adaptive_units.hpp"
#include "custom_dijkstra_call.hpp"
#include "generic_dijkstra.hpp"
#include "generic_label_creator.hpp"
#include "generic_label.hpp"
#include "generic_permanent.hpp"
#include "generic_tentative.hpp"
#include "generic_tracer.hpp"
#include "graph.hpp"
#include "stats.hpp"
#include "standard_dijkstra.hpp"
#include "standard_constrained_label_creator.hpp"
#include "standard_label_creator.hpp"
#include "standard_label.hpp"
#include "standard_permanent.hpp"
#include "standard_tentative.hpp"
#include "standard_tracer.hpp"
#include "yen_ksp.hpp"
#include "utils.hpp"

#include <boost/range.hpp>

#include <algorithm>
#include <climits>
#include <chrono>
#include <iterator>
#include <list>
#include <map>
#include <optional>
#include <set>
#include <tuple>

using namespace std;

routing::st_t routing::m_st = routing::st_t::none;

// Another routing algorithms to use.
set<routing::rt_t> routing::m_aras;

optional<unsigned> routing::m_K;

optional<cupath>
routing::set_up(graph &g, const demand &d)
{
  vertex src = d.first.first;

  // The maximal total number of units we found.
  unsigned nou = 0;

  // Itereate over the out edges of the src vertex.
  for(const auto &e: make_iterator_range(out_edges(src, g)))
    // The total number of units available on the edge.
    nou = std::max(nou, boost::get(boost::edge_nou, g, e));

  return set_up(g, d, CU(0, nou));
}

optional<cupath>
routing::set_up(graph &g, const demand &d, const CU &cu)
{
  vertex src = d.first.first;
  vertex dst = d.first.second;

  assert (src != dst);

  auto dr = search(g, d, cu, rt_t::dijkstra);

  for(const auto ara: m_aras)
    {
      // Another result.
      auto ar = search(g, d, cu, ara);

      if (!(!dr && !ar ||
            dr.value().first.count() == ar.value().first.count() &&
            get_path_cost(g, dr.value().second) ==
            get_path_cost(g, ar.value().second)))
        {
          cout << "dr = " << dr.value() << endl
               << "ar = " << ar.value() << endl;
          abort();
        }
    }

  if (dr)
    {
      bool status = set_up_path(g, dr.value());
      assert(status);
    }

  return dr;
}

optional<cupath>
routing::search(graph &g, const demand &d, const CU &cu, rt_t rt)
{
  using tp_t = chrono::time_point<chrono::high_resolution_clock>;

  tuple<int, int, int, optional<cupath> > p;

  tp_t t0 = std::chrono::system_clock::now();

  switch (rt)
    {
    case routing::rt_t::dijkstra:
      p = search_dijkstra(g, d, cu);
      break;

    case routing::rt_t::parallel:
      p = search_parallel(g, d, cu);
      break;

    case routing::rt_t::brtforce:
      p = search_brtforce(g, d, cu);
      break;

    case routing::rt_t::puyenksp:
      p = search_puyenksp(g, d, cu);
      break;

    default:
      abort();
    }

  tp_t t1 = std::chrono::system_clock::now();
  chrono::duration<double> dt = t1 - t0;

  stats::get().algo_perf(rt, dt.count(),
                         get<0>(p), get<1>(p), get<2>(p));

  return get<3>(p);
}

template <typename Units>
struct edge_has_units
{
  const graph *m_gp;
  const Units *m_units;

  edge_has_units(): m_gp(0), m_units(0)
  {
  }

  edge_has_units(const graph &g, const Units &units):
    m_gp(&g), m_units(&units)
  {
  }

  bool
  operator () (const edge &e) const
  {
    return boost::get(boost::edge_su, *m_gp, e).includes(*m_units);
  }
};

template <typename T>
bool
is_consistent(const T &C)
{
  // In this loop we make sure that the labels ls are consistent.
  for(auto const &ls: C)
    {
      // Make sure that the cost of the labels do not decrease.
      if (auto i = ls.begin(); i != ls.end())
        for (auto p = i; ++i != ls.end(); ++p)
          if (!(get_cost(*p) <= get_cost(*i)))
            return false;

      // There is no label, which is worse than some other label.
      // We're using the <= operator (not the < operator used for
      // sorting), and so we also make sure there are no equal labels.
      for (auto i = ls.begin(); i != ls.end(); ++i)
        for (auto j = i; ++j != ls.end();)
          if (*i <= *j || *j <= *i)
            return false;
    }

  return true;
}

CU
find_me_cu(const generic_permanent<graph, COST, CU> &P)
{
  // Find any label, so that we get a CU.
  for(const auto &vd: P)
    if (!vd.empty())
      return get_units(vd.front());

  return CU();
}

bool
is_optimal(const graph &g, vertex src, vertex dst, int ncu,
           generic_permanent<graph, COST, CU> &P)
{
  // These are the units for the filtered-graph search.
  while (CU fg_units = find_me_cu(P))
    {
      // Here we start searching for solutions with the standard
      // Dijkstra.  We filter the graph to leave only those edges that
      // have the "fg_units" CU.

      // The filtered graph type.
      using fg_type = boost::filtered_graph<graph,
                                            edge_has_units<CU>>;
      // The solution types.
      using per_type = standard_permanent<fg_type, COST>;
      using ten_type = standard_tentative<fg_type, COST>;

      // The edge predicate.
      edge_has_units<CU> ep(g, fg_units);
      // The filtered graph.
      fg_type fg(g, ep);

      // The permanent and tentative solutions.
      per_type FGP(boost::num_vertices(fg));
      ten_type FGT(boost::num_vertices(fg));

      // The label we start the search with.
      standard_label<fg_type, COST> fgl(0, edge(), src);
      // The reach of that modulation.
      COST r = adaptive_units<COST>::reach(ncu, fg_units.count());
      // The object that creates labels.
      standard_constrained_label_creator<fg_type, COST> fgc(fg, r);
      // Build the complete SPT.
      dijkstra(fg, FGP, FGT, fgl, fgc, graph::null_vertex());

      // -------------------------------------------------------------
      // We're sure that in FGP (the standard Dijkstra solution) there
      // are only the optimal solutions.  These results are the subset
      // of the results of the generic Dijkstra in P.
      for (auto pi = FGP.begin(); pi != FGP.end();)
        {
          // Is there a solution at pi?
          if (*pi)
            {
              // Now we know that there exists a shortest path from
              // vertex src to vertex v with fg_units units and
              // fg_cost cost.  This is certain.

              // This solution is for vertex v.
              vertex v = std::distance(FGP.begin(), pi);
              // The cost of reaching vertex v is fg_cost.
              auto fg_cost = get_cost(**pi);

              // Now we look what we've got from the generic Dijkstra
              // for node v.
              auto &ls = P[v];
              // There must at least one label, because the
              // filtered-graph search found a solution.
              assert(!ls.empty());

              // Iterate over the generic labels in ls.
              for (auto li = ls.begin(); li != ls.end();)
                {
                  // The generic Dijkstra label for vertex v.
                  const auto &gd_label = *li;
                  // The cost of the generic Dijkstra label.
                  const auto gd_cost = get_cost(gd_label);
                  // The units of the generic Dijkstra label.
                  const auto &gd_units = get_units(gd_label);

                  if (gd_units == fg_units)
                    {
                      // Bingo! The costs must be the same, because
                      // the units are the same.  We found the same
                      // path with two algorithms.
                      assert (gd_cost == fg_cost);
                      ls.erase(li++);
                    }
                  else
                    {
                      if (gd_units.includes(fg_units))
                        assert(gd_cost >= fg_cost);
                      else if (fg_units.includes(gd_units))
                        assert(fg_cost >= gd_cost);

                      ++li;
                    }
                }
            }
        }
    }

  return true;
}

tuple<int, int, int, optional<cupath> >
routing::search_dijkstra(const graph &g, const demand &d,
                         const CU &cu)
{
  vertex src = d.first.first;
  vertex dst = d.first.second;
  // The number of contiguous units.
  int ncu = d.second;

  assert (src != dst);

  // The accountant type.
  using acc_type = accountant<std::size_t>;
  // The generic permanent solution type.
  using per_type = generic_permanent<graph, COST, CU>;
  // The generic tentative solution type.
  using ten_type = generic_tentative<graph, COST, CU>;
  // The accounted generic permanent solution type.
  using acc_per_type = accounted_solution<per_type, acc_type>;
  // The accounted generic tentative solution type.
  using acc_ten_type = accounted_solution<ten_type, acc_type>;

  // The accountant finds the maximal number of labels used.
  acc_type acc;
  // The permanent and tentative solutions.
  acc_per_type P(acc, boost::num_vertices(g));
  acc_ten_type T(acc, boost::num_vertices(g));
  // The label we start the search with.
  generic_label<graph, COST, CU> l(0, CU(cu), edge(), src);
  // The creator of the labels.
  generic_label_creator<graph, COST, CU> c(g, ncu);
  // Run the search.
  dijkstra(g, P, T, l, c, dst);
  // The tracer.
  generic_tracer<graph, cupath, acc_per_type, CU> t(g, ncu);
  // Get the path.
  auto op = trace(P, dst, l, t);

  // Make sure that all the results in S and Q are consistent.
  assert(is_consistent(P));
  assert(is_consistent(T));
  // Make sure that all the results in S are optimal.  We're cleaning
  // up S, but that's OK, because it's no longer needed.
  assert(is_optimal(g, src, dst, ncu, P));

  // The number of costs, the number of edges, and the number of CUs
  // (units) equals to the number of labels, because a label has one
  // cost, one edge, and one CU.  We assume a cost takes a single
  // word, a label takes two words, and a CU takes two words.
  return make_tuple(acc.m_max, 2 * acc.m_max, 2 * acc.m_max,
                    std::move(op));
}

tuple<int, int, int, optional<cupath> >
routing::search_parallel(const graph &g, const demand &d, const CU &cu)
{
  vertex src = d.first.first;
  vertex dst = d.first.second;
  int min_units = d.second;

  // The max of edges and costs.
  std::size_t max_cae = 0;

  assert (src != dst);

  set<int> ncus = adaptive_units<COST>::ncus(min_units);

  // Here we store the result.
  optional<cupath> result;

  // Candidate SUs.
  for (int units: ncus)
    {
      // Get the candidate SUs (slots) with the given number of units.
      auto slots = get_candidate_slots(cu, units);

      // We have to go through all candidate SUs, because we don't
      // know which shall yield the shortest path.
      for (const CU &cu: slots)
        {
          edge_has_units<CU> ep(g, cu);

          // The filtered graph type.
          using fg_type = boost::filtered_graph<graph,
                                                edge_has_units<CU> >;

          // The filtered graph.
          fg_type fg(g, ep);

          // The accountant type.
          using acc_type = accountant<std::size_t>;

          // The standard permanent solution type.
          using per_type = standard_permanent<fg_type, COST>;
          // The standard tentative solution type.
          using ten_type = standard_tentative<fg_type, COST>;
          // The accounted standard permanent solution type.
          using acc_per_type = accounted_solution<per_type, acc_type>;
          // The accounted standard tentative solution type.
          using acc_ten_type = accounted_solution<ten_type, acc_type>;

          // Standard accountant.
          acc_type acc;
          acc_per_type P(acc, boost::num_vertices(fg));
          acc_ten_type T(acc, boost::num_vertices(fg));

          // The label we start the search with.
          standard_label<fg_type, COST> l(0, edge(), src);
          // The reach of that modulation.
          COST r = adaptive_units<COST>::reach(min_units, units);
          // The object that creates labels.
          standard_constrained_label_creator<fg_type, COST> c(fg, r);
          // Start the search.
          dijkstra(fg, P, T, l, c, dst);
          // The standard tracer.
          standard_tracer<fg_type, acc_per_type, path> t(fg);
          auto op = trace(P, dst, l, t);

          if (op && (!result ||
                     get_path_cost(g, op.value()) <
                     get_path_cost(g, result.value().second)))
            result = cupath(cu, op.value());

          if (max_cae < acc.m_max)
            max_cae = acc.m_max;
        }

      // If result found, stop searching for the next number of units.
      if (result)
        break;
    }

  // The number of costs and the number of edges equals to the number
  // of labels, because a label has one edge and one cost.  Every
  // search in iteratios above takes a single CU.  We assume a cost
  // takes a single word, a label takes two words, and a CU takes two
  // words.
  return make_tuple(max_cae, 2 * max_cae, 2, result);
}

// The adaptor class which keeps track of the max number of costs,
// edges and units stored in the priority queue.
template<typename T, typename C, typename F>
class my_priority_queue
{
  using qt = std::priority_queue<T, C, F>;

  // The queue.
  qt Q;
  
  // The number of costs to store.
  int costs = 0;
  // The number of edges to store.
  int edges = 0;
  // The number of units to store.
  int units = 0;

public:
  // The number of costs to store.
  int max_costs = 0;
  // The number of edges to store.
  int max_edges = 0;
  // The number of units to store.
  int max_units = 0;

  typename qt::const_reference top() const
  {
    return Q.top();
  }

  void push(T &&q)
  {
    // There is a single cost for every path pushed.
    ++costs;
    // Increase by the number of edges of the path pushed.
    edges += get<1>(q).first.size();
    // Increase by the number of units of the path pushed.
    units += get<1>(q).second.size();
    // Push the element as the r-value to Q.
    Q.push(std::move(q));

    if (max_costs + max_edges + max_units < costs + edges + units)
      {
        max_costs = costs;
        max_edges = edges;
        max_units = units;
      }
  }

  void pop()
  {
    // This is the element we are about to pop.
    auto const &q = Q.top();
    // There is a single cost for every path poped.
    --costs;
    // Decrease by the number of edges of the path poped.
    edges -= get<1>(q).first.size();
    // Decrease by the number of units of the path poped.
    units -= get<1>(q).second.size();
    // Pop the element.
    Q.pop();
  }

  bool empty()
  {
    return Q.empty();
  }
};

// The SU path.
typedef std::pair<SU, path> supath;

// This is the implementation of the algorithm from "Dynamic Routing
// and Spectrum Assignment in Spectrum-Flexible Transparent Optical
// Networks".  A path in the priority queue has its SU.
tuple<int, int, int, optional<cupath> >
routing::search_brtforce(const graph &g, const demand &d, const CU &cu)
{
  // This is the result.
  optional<cupath> result;

  vertex src = d.first.first;
  vertex dst = d.first.second;
  // The number of contiguous units.
  int ncu = d.second;

  assert (src != dst);

  using ew = boost::edge_weight_t;
  using wt = boost::property_map<graph, ew>::value_type;
  // The queue element has to store the destination vertex too,
  // because for the primer element, we will not be able to figure out
  // the vertex from the empty supath.
  using qe = std::tuple<wt, supath, vertex>;

  // This is the priority queue.
  // std::priority_queue<qe, std::vector<qe>, std::greater<qe> > Q;
  my_priority_queue<qe, std::vector<qe>, std::greater<qe> > Q;

  // Insert the primer.
  Q.push(make_tuple(0, supath(SU{cu}, {}), src));

  while (!Q.empty())
    {
      auto q = Q.top();
      Q.pop();
      wt c = get<0>(q);
      const supath &p = get<1>(q);
      vertex v = get<2>(q);

      if (v == dst)
        {
          int units = adaptive_units<COST>::units(ncu, c);
          // The selected CU.
          CU cu = select_cu(p.first, units);
          result = cupath(cu, p.second);
          break;
        }

      for(const auto &e: make_iterator_range(out_edges(v, g)))
        {
          // The target vertex of edge e.
          vertex t = boost::target(e, g);

          // We don't allow for loops.
          if (!vertex_in_path(g, p.second, t))
            {
              // The edge SU.
              const SU &e_su = boost::get(boost::edge_su, g, e);
              // The path SU.
              const SU &p_su = p.first;
              // The edge cost.
              wt ec = boost::get(boost::edge_weight, g, e);
              // The candidate cost.
              auto cc = c + ec;
              // The candidate SU.
              SU c_su = intersection(p_su, e_su);
              c_su.remove(adaptive_units<COST>::units(ncu, cc));
              if (!c_su.empty())
                {
                  // The candidate path.
                  supath cp = supath(std::move(c_su), p.second);
                  cp.second.push_back(e);
                  Q.push(make_tuple(cc, std::move(cp), t));
                }
            }
        }
    }

  // We assume a cost takes a single word, a label takes two words,
  // and a CU takes two words.
  return make_tuple(Q.max_costs, 2 * Q.max_edges, 2 * Q.max_units,
                    result);
}

tuple<int, int, int, optional<cupath> >
routing::search_puyenksp(const graph &g, const demand &d, const CU &cu)
{
  vertex src = d.first.first;
  vertex dst = d.first.second;
  // The number of contiguous units.
  int ncu = d.second;

  assert (src != dst);

  using ew = boost::edge_weight_t;
  using wt = boost::property_map<graph, ew>::value_type;
  using kr_type = boost::Result<wt, graph>;

  // The shortest paths - these we return.
  std::list<kr_type> A;
  std::set<kr_type> B;

  // In each iteration we produce the k-th shortest path.
  for (int k = 1; !m_K || k <= m_K.value(); ++k)
    {
      if (!yen_ksp(g, src, dst,
                   get(boost::edge_weight_t(), g),
                   get(boost::vertex_index_t(), g), A, B))
        break;

      // This is the k shortest path.
      const kr_type &r = A.back();

      // The cost of the path.
      COST c = get_path_cost(g, r.second);

      // This is the path SU.
      SU psu = find_path_su(g, r.second);
      // This is the candidate SU.
      SU csu = intersection(psu, SU{cu});
      // The number of required units at cost c.
      int units = adaptive_units<COST>::units(ncu, c);
      // Cut those CUs that don't have ncu units.
      csu.remove(units);

      if (!csu.empty())
        {
          // This is the selected CU.
          CU ecu = select_cu(csu, units);

          // The number of costs to store.
          int costs = A.size() + B.size();
          // The number of edges to store.
          int edges = 0;

          for (const auto &p: A)
            edges += p.second.size();

          for (const auto &p: B)
            edges += p.second.size();

          // We found a solution.
          return make_tuple(costs, edges, units,
                            cupath(ecu, r.second));
        }
    }

  // We should never get here, because we call the function only when
  // we know the solution exists.
  assert(false);
  // We put this return instruction here, so that the compile doesn't
  // complain.
  return make_tuple(0, 0, 0, cupath(CU(), path()));
}

routing::st_t
routing::st_interpret (const string &st)
{
  static const map <string, routing::st_t> st_map
  {{"first", routing::st_t::first},
   {"fittest", routing::st_t::fittest},
   {"random", routing::st_t::random}};
  return interpret ("spectrum selection type", st, st_map);
}

void
routing::set_K(optional<unsigned> K)
{
  m_K = K;
}

optional<unsigned>
routing::get_K()
{
  return m_K;
}

void
routing::set_st(st_t st)
{
  m_st = st;
}

void
routing::set_st(const string &st)
{
  m_st = st_interpret(st);
}

routing::st_t
routing::get_st()
{
  return m_st;
}

void
routing::add_another_algorithm(const routing::rt_t rt)
{
  m_aras.insert(rt);
}

bool
routing::set_up_path(graph &g, const cupath &p)
{
  boost::property_map<graph, boost::edge_su_t>::type
    sm = get(boost::edge_su_t(), g);

  for(const auto &e: p.second)
    sm[e].remove(p.first);

  return true;
}

void
routing::tear_down(graph &g, const cupath &p)
{
  boost::property_map<graph, boost::edge_su_t>::type
    sm = get(boost::edge_su_t(), g);

  // Iterate over the edges of the path.
  for(const auto &e: p.second)
    sm[e].insert(p.first);
}

CU
routing::select_cu(const CU &cu, int ncu)
{
  // This is the cunits.
  CU found;

  // For now we support only the first spectrum selection type.
  assert(m_st == st_t::first);

  switch (m_st)
    {
    case st_t::first:
      found = select_first(cu, ncu);
      break;

    case st_t::fittest:
      found = select_fittest(cu, ncu);
      break;

    case st_t::random:
      found = select_random(cu, ncu);
      break;

    default:
      assert(false);
    }

  // Make sure the CU has exactly ncu units.
  return CU(found.min(), found.min() + ncu);
}

CU
routing::select_first(const CU &cu, int ncu)
{
  assert(cu.count () >= ncu);
  return CU(cu.min(), cu.min() + ncu);
}

CU
routing::select_fittest(const CU &cu, int ncu)
{
  assert(false);
  return CU();
}

CU
routing::select_random(const CU &cu, int ncu)
{
  assert(false);
  return CU();
}

CU
routing::select_cu(const SU &su, int ncu)
{
  // For now we support only the first spectrum selection type.
  assert(m_st == st_t::first);

  assert(!su.empty());
  const CU &cu = *su.begin();
  return select_first(cu, ncu);
}

string
routing::to_string(routing::rt_t rt)
{
  static const map<routing::rt_t, string> t2s
  {{routing::rt_t::dijkstra, "dijkstra"},
   {routing::rt_t::parallel, "parallel"},
   {routing::rt_t::brtforce, "brtforce"},
   {routing::rt_t::puyenksp, "puyenksp"}};
  auto i = t2s.find(rt);
  assert(i != t2s.end());
  return i->second;
}
