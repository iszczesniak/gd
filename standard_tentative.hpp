#ifndef STANDARD_TENTATIVE_HPP
#define STANDARD_TENTATIVE_HPP

#include "standard_label.hpp"

#include <cassert>
#include <map>
#include <utility>

// The container type for storing both permanent and tentative labels.
template <typename Graph, typename Cost>
struct standard_tentative:
  public std::map<Vertex<Graph>, standard_label<Graph, Cost>>
{
  using label_type = standard_label<Graph, Cost>;
  using base_type = std::map<Vertex<Graph>, standard_label<Graph, Cost>>;

  template<typename T>
  void
  push(T &&l)
  {
    // The target vertex of the label.
    Vertex<Graph> t = get_target(l);
    // The iterator to the pair with the target vertex.
    auto i = base_type::find(t);
    // Just insert the label if the vertex t doesn't have one.
    if (i == base_type::end())
      base_type::operator[](t) = std::forward<T>(l);
    else
      {
        // We are about to replace the label for the target vertex t.
        // Make sure that the new label is of the lower cost.
        assert (get_cost(l) < get_cost(i->second));
        i->second = std::forward<T>(l);
      }
  }

  label_type
  pop()
  {
    // Find the set with the label of the lowest cost.
    auto qi =
      std::min_element(base_type::begin(), base_type::end(),
                       [](const auto &a, const auto &b)
                       {return get_cost(a.second) <
                        get_cost(b.second);});
    // Make sure we found the one.
    assert(qi != base_type::end());
    // Extract the node.
    auto nh = base_type::extract(qi);
    // Make sure we extracted a node.
    assert(nh);

    return std::move(nh.mapped());
  }
};

#endif // STANDARD_TENTATIVE_HPP
