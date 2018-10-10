#ifndef UTILS_HPP
#define UTILS_HPP

#include "generic_label.hpp"
#include "graph.hpp"
#include "standard_label.hpp"
#include "units.hpp"

#include <boost/graph/connected_components.hpp>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/accumulators/statistics/mean.hpp>
#include <boost/accumulators/statistics/variance.hpp>

#include <cassert>
#include <iostream>
#include <list>
#include <map>
#include <random>
#include <set>
#include <string>
#include <vector>

namespace ba = boost::accumulators;

// The accumulator with double values.
typedef ba::accumulator_set<double, ba::stats<ba::tag::min,
                                              ba::tag::mean,
                                              ba::tag::max,
                                              ba::tag::variance> > dbl_acc;

/**
 * Generate a random number from min to max, including both min and
 * max.
 */
template<typename E>
int
get_random_int(int min, int max, E &eng)
{
  std::uniform_int_distribution<> d(min, max);
  return d(eng);
}

// Calculate the cost of the path.
template<typename Graph, typename Container>
typename boost::property_map<Graph, boost::edge_weight_t>::value_type
get_path_cost(const Graph &g, const Container &con)
{
  using ew = boost::edge_weight_t;
  using w = typename boost::property_map<Graph, ew>::value_type;
  using edge = typename Graph::edge_descriptor;

  w c = std::accumulate(con.begin(), con.end(), w(),
                        [g](const w &c, const edge &e)
                        {
                          w o = boost::get(boost::edge_weight, g, e);
                          return c + o;
                        });
  return c;
}

/**
 * Interpret a string, and return the matched enum.
 */
template <typename T>
T
interpret(const std::string &name, const std::string &text,
          const std::map <std::string, T> &m)
{
  auto i = m.find (text);

  if (i == m.end ())
    {
      std::cerr << "Wrong value of " << name << ".  Choose one of:";
      for (auto &p: m)
        std::cerr << " " << p.first;
      std::cerr << std::endl;
      exit (1);
    }

  return i->second;
}

/**
 * Find the SU that is available along the path.
 */
SU
find_path_su(const graph &g, const path &p);

/**
 * This is the << operator for a pair.
 */
template <typename A, typename B>
std::ostream &operator << (std::ostream &os, const std::pair<A, B> &p)
{
  os << "pair(" << p.first << ", " << p.second << ")";
  return os;
}

/**
 * This is the << operator for a cupath.
 */
std::ostream &operator << (std::ostream &os, const cupath &p);

/**
 * This is the << operator for a vector.
 */
template <typename T>
std::ostream &operator << (std::ostream &os, const std::vector<T> &v)
{
  typename std::vector<T>::const_iterator i = v.begin();

  os << "[";

  while(i != v.end())
    {
      os << *i;
      if (++i != v.end())
        os << ", ";
    }

  os << "]";

  return os;
}

/**
 * This is the << operator for a set.
 */
template <typename T>
std::ostream &operator << (std::ostream &os, const std::set<T> &v)
{
  typename std::set<T>::const_iterator i = v.begin();

  os << "{";

  while(i != v.end())
    {
      os << *i;
      if (++i != v.end())
        os << ", ";
    }

  os << "}";

  return os;
}

/**
 * This is the << operator for a list.
 */
template <typename T>
std::ostream &operator << (std::ostream &os, const std::list<T> &v)
{
  typename std::list<T>::const_iterator i = v.begin();

  os << "(";

  while(i != v.end())
    {
      os << *i;
      if (++i != v.end())
        os << ", ";
    }

  os << ")";

  return os;
}

/**
 * This is the << operator for a map.
 */
template <typename K, typename V>
std::ostream &operator << (std::ostream &os, const std::map<K, V> &m)
{
  for(auto const &e: m)
    os << e.first << " -> " << e.second << std::endl;

  return os;
}

/**
 * True if the graph is connected.
 *
 * @return: true if the graph is connected, false otherwise.
 */
template<typename G>
bool
is_connected(const G &g)
{
  int c[boost::num_vertices(g)];

  // "num" is the number of connected components.
  int num = boost::connected_components(g, c);

  return num == 1;
}

/**
 * Get a random element from a container.
 */
template <typename C, typename E>
auto
get_random_element(const C &c, E &eng)
{
  assert(!c.empty());

  typename C::const_iterator i = c.begin();
  std::advance(i, get_random_int(0, c.size() - 1, eng));

  return *i;
}

/**
 * Generate a random node pair.
 */
template<typename G, typename E>
std::pair<typename G::vertex_descriptor,
	  typename G::vertex_descriptor>
random_node_pair(const G &g, E &eng)
{
  typedef typename G::vertex_descriptor vertex;
  typedef typename G::vertex_iterator vertex_i;

  vertex_i begin = vertices(g).first;
  int n = num_vertices(g);

  int src_n = get_random_int(0, n - 1, eng);
  vertex_i src_i = begin;
  std::advance(src_i, src_n);
  vertex src = *src_i;

  // Choose any number between 0 and n - 1, but not src_n.
  int dst_n = get_random_int(0, n - 2, eng);
  if (dst_n == src_n)
    dst_n = n - 1;
  vertex_i dst_i = begin;
  std::advance(dst_i, dst_n);
  vertex dst = *dst_i;

  return std::make_pair(src, dst);
}

template<typename G>
double
calculate_utilization(const G &g)
{
  ba::accumulator_set<double, ba::stats<ba::tag::mean> > load_acc;

  typename G::edge_iterator ei, ee;
  for (tie(ei, ee) = edges(g); ei != ee; ++ei)
    {
      // Total available units.
      int tsc = boost::get(boost::edge_nou, g, *ei);
      // Currenlty available units.
      int asc = boost::get(boost::edge_su, g, *ei).count();
      // The link load.
      double load = double(tsc - asc) / tsc;
      load_acc(load);
    }

  return ba::mean(load_acc);
}

bool
vertex_in_path(const graph &g, const path &p, vertex v);

/**
 * Sets the units property on edges.
 */
template<typename G>
void
set_units(G &g, unsigned units)
{
  SU su = {{0, units}};

  // Total units property map.
  typename boost::property_map<G, boost::edge_nou_t>::type
    tpm = get(boost::edge_nou_t(), g);
  // Available units property map.
  typename boost::property_map<G, boost::edge_su_t>::type
    apm = get(boost::edge_su_t(), g);

  typename G::edge_iterator ei, ee;
  for (tie(ei, ee) = edges(g); ei != ee; ++ei)
    {
      tpm[*ei] = units;
      apm[*ei] = su;
    }
}

// For the shortest paths between all node pairs, calculate the
// statistics for hops and lengths.
void
calc_sp_stats(const graph &g, dbl_acc &hop_acc, dbl_acc &len_acc);

// Calculate the mean connection arrival time for the given mean
// number of hops of a shortest path (mnh), the mean holding time
// (mht) of a connection, the mean number of units of a connection,
// and the offered load (ol).
double
calc_mcat(const graph &g, double mnh, double mht, double mnu,
          double ol);

bool
load_graphviz(const std::string &file_name, graph &g);

#endif /* UTILS_HPP */
