#include "connection.hpp"
#include "routing.hpp"
#include "utils.hpp"

#include <algorithm>
#include <iostream>
#include <optional>
#include <utility>

using namespace std;

int connection::counter = 0;

connection::connection(graph &g): m_g(g), m_id(counter++)
{
}

connection::~connection()
{
  if (is_established())
    tear_down();
}

const demand &
connection::get_demand() const
{
  return m_d;
}

bool
connection::is_established() const
{
  return m_p.has_value();
}

int
connection::get_len() const
{
  assert(is_established());
  return get_path_cost(m_g, m_p.value().second);
}

int
connection::get_nol() const
{
  assert(is_established());
  return m_p.value().second.size();
}

int
connection::get_ncu() const
{
  assert(is_established());
  return m_p.value().first.count();
}

bool
connection::establish(const demand &d)
{
  // Make sure the connection is not established.
  assert(!is_established());

  // Set up the demand.
  m_p = routing::set_up(m_g, d);

  // If successful, remember the demand.
  if (m_p)
    m_d = d;

  return is_established();
}

void
connection::tear_down()
{
  assert(is_established());
  routing::tear_down(m_g, m_p.value());
  m_p.reset();
}
