#ifndef STANDARD_TENTATIVE_HPP
#define STANDARD_TENTATIVE_HPP

#include "standard_label.hpp"

#include <cassert>
#include <memory>
#include <queue>
#include <vector>

// The container type for storing the standard tentative labels.  A
// vertex can have a shared (with the priority queue) label or not.
// For each vertex we store a shared_ptr to the label, because we
// create a weak_ptr in the priority queue.
template <typename Graph, typename Cost>
struct standard_tentative
{
  // That's the label type we're using.
  using label_t = standard_label<Graph, Cost>;
  // The type of data a vertex has.
  using vd_t = std::shared_ptr<label_t>;
  // The type of the vector of vertex data.
  using vovd_t = std::vector<vd_t>;
  // The size type of the vovd_t.
  using size_type = typename vovd_t::size_type;

  // The vector of vertex data.
  vovd_t m_vovd;

  // The priority queue element type.
  using pqet = std::pair<Cost, std::weak_ptr<label_t>>;

  // The priority queue.
  std::priority_queue<pqet, std::vector<pqet>,
                      std::function<bool(const pqet &,
                                         const pqet &)>>
    m_pq{[](const auto &a, const auto &b)
      {return ! (a.first < b.first);}};

  standard_tentative(size_type count): m_vovd(count)
  {
  }

  // This function pushes a new label.
  template <typename T>
  void
  push(T &&l)
  {
    // The target vertex of the label.
    const auto &t = get_target(l);
    // There should be no label for vertex t.
    assert(!m_vovd[t]);
    // Make a shared_ptr.
    m_vovd[t] = std::make_shared<label_t>(l);
    // Push the weak_ptr for the label into the priority queue.
    m_pq.push({get_cost(l), m_vovd[t]});
  }

  bool
  empty() const
  {
    return m_pq.empty();
  }

  auto
  pop()
  {
    // Make sure the queue is not empty.
    assert(!m_pq.empty());
    // Get a shared_ptr from the element on top.
    auto sp = m_pq.top().second.lock();
    // The top element must have a label, and so sp cannot be nullptr.
    // In the queue, there might be an element whose label was
    // discarded, because we found a label of a smaller cost.  But
    // then the label of the smaller cost will come first, and the
    // element of the queue with the discarded label would come later.
    assert(sp);
    // Pop the element.
    m_pq.pop();

    // Clean up the trailing elements, whose labels were discarded.
    while(!m_pq.empty() && m_pq.top().second.expired())
      m_pq.pop();

    // This is the label we return.
    const auto l = std::move(*sp);
    // Make sure we've got the label in the right place.
    assert(m_vovd[get_target(l)]);
    // We remove the label from the vector.
    m_vovd[get_target(l)].reset();

    return l;
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

#endif // STANDARD_TENTATIVE_HPP
