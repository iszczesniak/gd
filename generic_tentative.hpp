#ifndef GENERIC_TENTATIVE_HPP
#define GENERIC_TENTATIVE_HPP

#include "generic_label.hpp"

#include <cassert>
#include <queue>
#include <set>
#include <vector>

// The container type for storing the generic tentative labels.  A
// vertex can have many shared (with the priority queue) labels or
// none.  For each vertex we store a container or shared_ptrs to
// labels, because we create a weak_ptr in the priority queue.
template <typename Graph, typename Cost, typename Units>
struct generic_tentative
{
  // That's the label type we're using.
  using label_t = generic_label<Graph, Cost, Units>;
  // The type of data a vertex has.
  using vd_t = std::set<label_t>;
  // The type of the vector of vertex data.
  using vovd_t = std::vector<vd_t>;
  // The size type of the vovd_t.
  using size_type = typename vovd_t::size_type;
  // The type of the vertex descriptor.
  using vertex_t = Vertex<Graph>;

  // The vector of vertex data.
  vovd_t m_vovd;

  // The priority queue element type.
  using pqet = std::pair<Cost, vertex_t>;

  // The priority queue.
  std::set<pqet> m_pq;

  // Vetext to cost reverse look up of the priority queue elements.
  std::vector<std::optional<Cost>> m_v2c;

  // The constructor builds a vector of data for each vertex.
  generic_tentative(size_type count): m_vovd(count), m_v2c(count)
  {
  }

  // This function pushes a new label.
  template <typename T>
  void
  push(T &&l)
  {
    vertex_t t = get_target(l);
    Cost c = get_cost(l);
    auto &vd = m_vovd[t];
    auto [i, s] = vd.insert(std::forward<T>(l));
    // Make sure the insertion was successful.
    assert(s);
    if (i == vd.begin())
      {
        // There already can be an element in the queue for t.
        auto &o = m_v2c[t];
        if (o)
          // Erase the former element from the queue.
          m_pq.erase({*o, t});
        m_pq.insert({c, t});
        o = c;
      }
  }

  bool
  empty() const
  {
    return m_pq.empty();
  }

  label_t
  pop()
  {
    assert(!m_pq.empty());
    const auto [c, t] = *m_pq.begin();
    m_pq.erase(m_pq.begin());
    auto &vd = m_vovd[t];
    assert(!vd.empty());
    auto nh = vd.extract(vd.begin());
    assert(get_cost(nh.value()) == c);
    auto &o = m_v2c[t];
    // If there is other label for t, put it into the queue.
    if (!vd.empty())
      {
        const auto &nc = get_cost(*vd.begin());
        assert(get_target(*vd.begin()) == t);
        m_pq.insert({nc, t});
        o = nc;
      }
    else
      o.reset();

    return std::move(nh.value());
  }

  // This is a const member, because we allow the random access, but
  // disallow the modification of the element.
  const vd_t &
  operator[](size_type i) const
  {
    return m_vovd[i];
  }

  // This operator returns a reference to the vertex data, which we
  // can modify.
  vd_t &
  operator[](size_type i)
  {
    return m_vovd[i];
  }
};

#endif // GENERIC_TENTATIVE_HPP
