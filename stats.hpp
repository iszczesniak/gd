#ifndef STATS_HPP
#define STATS_HPP

#include "cli_args.hpp"
#include "connection.hpp"
#include "event.hpp"
#include "graph.hpp"
#include "module.hpp"
#include "routing.hpp"
#include "sim.hpp"
#include "traffic.hpp"

#include <chrono>
#include <map>
#include <optional>
#include <vector>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

namespace ba = boost::accumulators;

class client;

class stats: public module<sim>
{
  // The singleton of the class.
  static stats *singleton;

  // The traffic of the run.
  const traffic &m_tra;

  // The arguments of the run.
  const cli_args &m_args;

  // The number of instantaneous measurements of the network state.
  const int nom = 100;

  // The time difference for taking the instantaneous measurements.
  const sim::time_type m_dt;

  // The accumulator type with double values.
  typedef ba::accumulator_set<double, ba::features<ba::tag::count,
                                                   ba::tag::mean,
                                                   ba::tag::max>> dbl_acc;

  // The utilization.
  dbl_acc m_utilization;

  // The probability of establishing a connection.
  dbl_acc m_pec;
  // The length of the established connection.
  dbl_acc m_lenec;
  // The number of links of the established connection.
  dbl_acc m_nolec;
  // The number of contiguous units of the established connection.
  dbl_acc m_ncuec;

  // The costs statistics.
  std::map<routing::rt_t, dbl_acc> m_costs;
  // The edges statistics.
  std::map<routing::rt_t, dbl_acc> m_edges;
  // The sas statistics.
  std::map<routing::rt_t, dbl_acc> m_units;

  // The number of connections served.
  dbl_acc m_conns;
  // The capacity served.
  dbl_acc m_capser;
  // The number of fragments.
  dbl_acc m_frags;

public:
  stats(const cli_args &, const traffic &);

  ~stats();

  static stats &
  get();

  void
  schedule(const double t);

  void
  operator()(const double t);

  // Report the establishment status.
  void
  established(const bool status);

  // Report the established connection.
  void
  established_conn(const connection &conn);

  // Report the algorithm performance.
  void
  algo_perf(const routing::rt_t rt, const double dt,
            const int costs, const int edges, const int units);

private:
  // Calculate the average number of fragments on a link.
  double
  calculate_frags();
};

#endif
