#ifndef GENERIC_TENTATIVE_HPP
#define GENERIC_TENTATIVE_HPP

#include "generic_label.hpp"

#include <cassert>
#include <map>
#include <utility>

template <typename Graph, typename Cost, typename Units>
using generic_labels = std::set<generic_label<Graph, Cost, Units>>;

template <typename Graph, typename Cost, typename Units>
struct generic_tentative:
  public std::map<Vertex<Graph>, generic_labels<Graph, Cost, Units>>
{
  using base_type = std::map<Vertex<Graph>,
			     generic_labels<Graph, Cost, Units>>;
  using label_type = generic_label<Graph, Cost, Units>;
  using labels_type = generic_labels<Graph, Cost, Units>;
  using vertex_type = Vertex<Graph>;

  template <typename T>
  void
  push(T &&l)
  {
    // The target of the label.
    vertex_type trg = get_target(l);
    // The target labels.
    labels_type &trg_labels = base_type::operator[](trg);
    // Insert the label.
    bool status = trg_labels.insert(std::forward<T>(l)).second;
    // The insertion should always succeed.
    assert(status);
  }

  label_type
  pop()
  {
    // Find the set with the label of the lowest cost.
    auto qi =
      std::min_element(base_type::begin(), base_type::end(),
		       [](const auto &a, const auto &b)
		       {return *a.second.begin() < *b.second.begin();});
    // Make sure we found the one.
    assert(qi != base_type::end());
    // The source labels we move from.
    labels_type &src_labels = qi->second;
    // The iterator to the label we move.
    auto li = src_labels.cbegin();

    // Extract the node.
    auto nh = src_labels.extract(li);
    // Make sure we extracted the node.
    assert(nh);
    
    // Remove the src_labels if it is empty.
    if (src_labels.empty())
      base_type::erase(qi);

    return std::move(nh.value());
  }
};

#endif // GENERIC_TENTATIVE_HPP
