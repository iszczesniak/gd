#include "utils.hpp"

#include "cunits.hpp"
#include "graph.hpp"

#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/range.hpp>

#include <algorithm>
#include <iterator>
#include <list>
#include <set>

using namespace std;

SU
find_path_su(const graph &g, const path &p)
{
  SU su;
  
  path::const_iterator i = p.begin();

  if (i != p.end())
    {
      su = boost::get(boost::edge_su, g, *i);

      while(++i != p.end())
        su = intersection(su, boost::get(boost::edge_su, g, *i));
    }

  return su;
}

// The function for sorting the list of sets.
static bool
stlos(const set<vertex> &s1, const set<vertex> &s2)
{
  return s1.size() > s2.size();
}

// This is the << operator for a cupath.
std::ostream &
operator << (std::ostream &os, const cupath &p)
{
  os << "cupath(" << p.first << ", " << p.second << ")";
  return os;
}

bool
vertex_in_path(const graph &g, const path &p, vertex v)
{
  for (const edge &e: p)
    if (source(e, g) == v || target(e, g) == v)
      return true;

  return false;
}

void
calc_sp_stats(const graph &g, dbl_acc &hop_acc, dbl_acc &len_acc)
{
  // Calculate stats for shortest paths.
  for (vertex src: boost::make_iterator_range(boost::vertices(g)))
    {
      vector<int> dist(num_vertices(g));
      vector<vertex> pred(num_vertices(g));

      boost::dijkstra_shortest_paths
        (g, src,
         boost::predecessor_map(&pred[0]).distance_map(&dist[0]));

      for (vertex dst: boost::make_iterator_range(boost::vertices(g)))
        if (src != dst)
          {
            // Make sure the path was found.
            assert(pred[dst] != dst);

            // Record the number of hops.
            int hops = 0;
            vertex c = dst;
            while(c != src)
              {
                c = pred[c];
                ++hops;
              }
            hop_acc(hops);

            // Record the path length.
            len_acc(dist[dst]);
          }
    }
}

double
calc_mcat(const graph &g, double mnh, double mht, double mnu, double ol)
{
  // The network capacity, i.e. the number of units of all links.
  double cap = 0;
  for (const auto &e: boost::make_iterator_range(edges (g)))
    cap += boost::get(boost::edge_nou, g, e);

  // The mean connection arrival time.
  double mcat = mnh * mht * mnu / (ol * cap);

  return mcat;
}

bool
load_graphviz(const string &file_name, graph &g)
{
  ifstream is(file_name);

  if (!is)
    {
      cerr << "Error opening a graphviz file '" << file_name << "'.\n";
      return false;
    }

  boost::dynamic_properties dp;

  dp.property("node_id", get(boost::vertex_name, g));
  dp.property("weight", get(boost::edge_weight, g));

  bool result = read_graphviz(is, g, dp);

  is.close();

  return result;
}
