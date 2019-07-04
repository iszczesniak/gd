#ifndef GENERIC_PERMANENT_HPP
#define GENERIC_PERMANENT_HPP

#include "generic_label.hpp"

#include <utility>
#include <vector>

// The container type for storing permanent generic labels.  A vertex
// can have many labels or none, so we store them in a container.
template <typename Graph, typename Cost, typename Units>
struct generic_permanent:
  std::vector<std::vector<generic_label<Graph, Cost, Units>>>
{
  // That's the label type we're using.
  using label_t = generic_label<Graph, Cost, Units>;
  // The type of data a vertex has.
  using vd_t = std::vector<label_t>;
  // The type of the vector of vertex data.
  using vovd_t = std::vector<vd_t>;
  // That's the base class.
  using base = vovd_t;

  // The size type of the vovd_t.
  using size_type = typename vovd_t::size_type;

  generic_permanent(size_type count): base(count)
  {
  }

  // Pushes a new label, and returns a reference to it.
  template <typename T>
  const label_t &
  push(T &&l)
  {
    // The target vertex of the label.
    const auto &t = get_target(l);
    // Push the label back.
    operator[](t).push_back(std::forward<T>(l));

    return operator[](t).back();
  }

  // This is a const member, because we allow the random access, but
  // disallow the modification of the element.
  const vd_t &
  operator[](size_type i) const
  {
    return operator[](i);
  }
};

#endif // GENERIC_PERMANENT_HPP
