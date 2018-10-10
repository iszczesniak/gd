#include "traffic.hpp"

#include <list>

using namespace std;

traffic::traffic(double mcat, double mht, double mnu):
  m_catd(1 / mcat), m_mht(mht), m_mnu(mnu), idc()
{
  schedule(0);
}

traffic::~traffic()
{
  for(auto c: cs)
    delete c;

  delete_clients();
}

int
traffic::nr_clients() const
{
  return cs.size();
}

void
traffic::operator()(double t)
{
  // Before we create new clients, we delete those clients that
  // requested deletion.
  delete_clients();
  
  // We are creating a client, but we ain't doing anything with the
  // pointer we get!  It's so, because it's up to the client to
  // register itself with the traffic.
  new client(m_mht, m_mnu, *this);
  schedule_next(t);
}

void
traffic::schedule_next(double t)
{
  double dt = m_catd(m_rne);
  schedule(t + dt);
}

void
traffic::insert(client *c)
{
  cs.insert(c);
}

void
traffic::erase(client *c)
{
  cs.erase(c);
}

void
traffic::delete_me_later(client *c)
{
  dl.push(c);
}

int
traffic::capacity_served() const
{
  int capacity = 0;

  // Iterate over all clients.
  for(const client *cli: cs)
    {
      const connection &c = cli->get_connection();
      int ncu = c.get_ncu();
      int len = c.get_len();
      capacity += len * ncu;
    }

  return capacity;
}

void
traffic::delete_clients()
{
  while(!dl.empty())
    {
      delete dl.front();
      dl.pop();
    }
}
