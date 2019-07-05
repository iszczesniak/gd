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
struct standard_tentative:
  std::vector<std::shared_ptr<standard_label<Graph, Cost>>>
{
  // That's the label type we're using.
  using label_t = standard_label<Graph, Cost>;
  // The type of data a vertex has.
  using vd_t = std::shared_ptr<label_t>;
  // The type of the vector of vertex data.
  using base = std::vector<vd_t>;
  // The size type of the vovd_t.
  using size_type = typename base::size_type;

  // The priority queue element type.
  using pqet = std::pair<Cost, std::weak_ptr<label_t>>;

  // The priority queue.
  std::priority_queue<pqet, std::vector<pqet>,
                      std::function<bool(const pqet &,
                                         const pqet &)>>
    m_pq{[](const auto &a, const auto &b)
      {return ! (a.first < b.first);}};

  standard_tentative(size_type count): base(count)
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
    assert(!base::operator[](t));
    // Make a shared_ptr.
    base::operator[](t) = std::make_shared<label_t>(l);
    // Push the weak_ptr for the label into the priority queue.
    m_pq.push({get_cost(l), base::operator[](t)});
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
    assert(base::operator[](get_target(l)));
    // We remove the label from the vector.
    base::operator[](get_target(l)).reset();

    return l;
  }
};

#endif // STANDARD_TENTATIVE_HPP
