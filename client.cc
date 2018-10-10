#include "client.hpp"

#include "simulation.hpp"
#include "stats.hpp"
#include "traffic.hpp"
#include "utils.hpp"

#include <boost/property_map/property_map.hpp>

#include <map>
#include <utility>

using namespace std;

client::client(double mht, double mnu, traffic &tra):
  m_htd(1 / mht), m_nud(mnu - 1),
  conn(m_mdl), st(stats::get()), tra(tra)
{
  // Try to setup the connection.
  if (set_up())
    {
      // Register the client with the traffic.
      tra.insert(this);
      // Holding time.
      double ht = m_htd(m_rne);
      // Tear down time.
      tdt = now() + ht;
      schedule(tdt);
    }
  else
    // We didn't manage to establish the connection, and so the client
    // should be deleted.
    tra.delete_me_later(this);
}

void client::operator()(double t)
{
  destroy();
}

bool client::set_up()
{
  // The new demand.
  demand d;
  // The demand end nodes.
  d.first = random_node_pair(m_mdl, m_rne);
  // The number of units the signal requires.  It's Poisson + 1.
  d.second = m_nud(m_rne) + 1;

  // Set up the connection.
  bool status = conn.establish(d);

  // Report whether the connection was established or not.
  st.established(status);

  // If established, report the connection.
  if (status)
    st.established_conn(conn);

  return status;
}

const connection &
client::get_connection() const
{
  return conn;
}

void
client::destroy()
{
  assert(conn.is_established());
  conn.tear_down();
  tra.erase(this);
  tra.delete_me_later(this);
}
