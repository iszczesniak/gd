#ifndef TRAFFIC_HPP
#define TRAFFIC_HPP

#include "client.hpp"
#include "graph.hpp"
#include "module.hpp"
#include "sim.hpp"

#include <queue>
#include <random>
#include <set>

class traffic: public module<sim>
{
  // The set of active clients.
  std::set<client *> cs;

  // The queue of clients to delete later.
  std::queue<client *> dl;

  // The ID counter.
  int idc;

  // The client arrival time distribution.
  std::exponential_distribution<> m_catd;

  // The mean holding time.
  double m_mht;

  // The mean number of units.
  double m_mnu;

  // Shortest distances.
  mutable std::map<npair, int> sd;

public:
  traffic(double mcat, double mht, double mnu);

  ~traffic();

  // Processes the event and changes the state of the client.
  void operator()(double t);

  // Return the number of clients.
  int nr_clients() const;

  // Insert the client to the traffic.
  void insert(client *);

  // Remote the client from the traffic.
  void erase(client *);

  // Delete this client later.
  void delete_me_later(client *);

  // Calculate the capacity currently served, which is defined as
  // \sum_{i = connections} ncu_i * sp_i, where ncu_i is the number of
  // units of connection i, and sp_i is the length of the shortest
  // path between end nodes of connection i.
  int
  capacity_served() const;

private:
  void schedule_next(double);
  void delete_clients();
};

#endif /* TRAFFIC_HPP */
