#include "client.hpp"
#include "routing.hpp"
#include "stats.hpp"
#include "utils.hpp"

#include <boost/range.hpp>

#include <cmath>
#include <iostream>
#include <sstream>

using namespace std;

// The singleton of the class.  The compiler initializes it to null.
stats *stats::singleton;

stats::stats(const cli_args &args, const traffic &tra):
  m_args(args), m_tra(tra),
  m_dt((args.sim_time - args.kickoff) / nom)
{
  assert(!singleton);
  singleton = this;

  // We start collecting the utilization stats at the kickoff time.
  schedule(args.kickoff);
}

template <typename T>
void
report(const string &txt, const T &v)
{
  cout << txt << " " << v << endl;
}

stats::~stats()
{
  // The population name.
  report("population", m_args.population);

  // The network utilization.
  report("utilization", ba::mean(m_utilization));

  // The probability of establishing a connection.
  report("pec", ba::mean(m_pec));
  // The mean length of an established connection.
  report("lenec", ba::mean(m_lenec));
  // The mean number of links of an established connection.
  report("nolec", ba::mean(m_nolec));
  // The mean number of contiguous units of an established connection.
  report("ncuec", ba::mean(m_ncuec));

  // The algorithm statistics.
  for (const auto &e: m_costs)
    {
      // The routing type.
      auto rt = e.first;
      const string prefix = routing::to_string(rt) + '_';
      report(prefix + "mean_costs", ba::mean(m_costs[rt]));
      report(prefix + "max_costs", ba::max(m_costs[rt]));
      report(prefix + "mean_edges", ba::mean(m_edges[rt]));
      report(prefix + "max_edges", ba::max(m_edges[rt]));
      report(prefix + "mean_units", ba::mean(m_units[rt]));
      report(prefix + "max_units", ba::max(m_units[rt]));
    }

  // The number of currently active connections.
  report("conns", ba::mean(m_conns));
  // The capacity served.
  report("capser", ba::mean(m_capser));
  // The mean number of fragments on links.
  report("frags", ba::mean(m_frags));
}

stats &
stats::get()
{
  return *singleton;
}

void
stats::operator()(const double st)
{
  // The current network utilization.
  m_utilization(calculate_utilization(m_mdl));
  // The number of connections served.
  m_conns(m_tra.nr_clients());
  // The capacity served.
  m_capser(m_tra.capacity_served());
  // The number of fragments.
  m_frags(calculate_frags());

  schedule(st);
}

// Schedule the next event based on the current time 0.
void
stats::schedule(const double t)
{
  // We call the stats every second.
  module::schedule(t + m_dt);
}

void
stats::established(const bool status)
{
  if (m_args.kickoff <= now())
    m_pec(status);
}

void
stats::established_conn(const connection &conn)
{
  if (m_args.kickoff <= now())
    {
      int len = conn.get_len();
      int nol = conn.get_nol();
      int ncu = conn.get_ncu();

      m_lenec(len);
      m_nolec(nol);
      m_ncuec(ncu);
    }
}

void
stats::algo_perf(const routing::rt_t rt, const double dt,
                 const int costs, const int edges, const int units)
{
  if (m_args.kickoff <= now())
    {
      cerr << routing::to_string(rt) << " " << dt << endl;
      m_costs[rt](costs);
      m_edges[rt](edges);
      m_units[rt](units);
    }
}

double
stats::calculate_frags()
{
  dbl_acc frags;

  // Iterate over all edges.
  graph::edge_iterator ei, ee;
  for (tie(ei, ee) = boost::edges(m_mdl); ei != ee; ++ei)
    {
      const edge e = *ei;
      const SU &su = boost::get(boost::edge_su, m_mdl, e);
      int f = su.size();
      frags(f);
    }

  return ba::mean(frags);
}
