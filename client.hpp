#ifndef CLIENT_HPP
#define CLIENT_HPP

#include "connection.hpp"
#include "module.hpp"
#include "sim.hpp"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

#include <random>
#include <utility>

namespace ba = boost::accumulators;

class stats;
class traffic;

/**
 * The client class.  This class takes care of reporting the stats.
 */
class client: public module<sim>
{
  // The client ID.
  int id;

  // The tear down time.
  double tdt;

  // The holding time distribution.
  std::exponential_distribution<> m_htd;

  // The number of units distribution.
  std::poisson_distribution<> m_nud;

  // The connection.
  connection conn;

  // The traffic object the client belongs to.
  traffic &tra;
  
  // The statistics object to which we report.
  stats &st;

public:
  client(double mht, double mnu, traffic &tra);
  
  // Processes the event and changes the state of the client.
  void operator()(double t);

  const connection &
  get_connection() const;

private:
  bool set_up();
  void destroy();
};

#endif /* CLIENT_HPP */
