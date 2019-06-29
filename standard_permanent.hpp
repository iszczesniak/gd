#ifndef STANDARD_PERMANENT_HPP
#define STANDARD_PERMANENT_HPP

#include "standard_label.hpp"

#include <cassert>
#include <optional>
#include <utility>
#include <vector>

// We introduce myoptional only to have the empty function.
template <typename T>
struct myoptional: public std::optional<T>
{
  // Use the constructors of std::optional.
  using base = std::optional<T>;
  using base::base;

  bool
  empty() const
  {
    return !static_cast<bool>(*this);
  }
};

// The container type for storing permanent standard labels.  A vertex
// can have a label or not, and so we make the label optional.  We go
// for the std::optional instead of std::unique_ptr, because optional
// stores its value inside, and so the labels are allocated in-place
// and not in a remote part.
template <typename Graph, typename Cost>
struct standard_permanent
{
  // That's the label type we're using.
  using label_t = standard_label<Graph, Cost>;
  // The type of data a vertex has.
  using vd_t = myoptional<label_t>;
  // The type of the vector of vertex data.
  using vovd_t = std::vector<vd_t>;
  // The size type of the vovd_t.
  using size_type = typename vovd_t::size_type;

  // The vector of vertex data.
  vovd_t m_vovd;

  standard_permanent(size_type count): m_vovd(count)
  {
  }

  // Pushes a new label, and returns a reference to it.
  template <typename T>
  const label_t &
  push(T &&l)
  {
    // The target vertex of the label.
    const auto &t = get_target(l);
    // There should be no label for vertex t.
    assert(!m_vovd[t]);
    // Set the value.
    m_vovd[t] = std::forward<T>(l);

    return *m_vovd[t];
  }

  // This is a const member, because we allow the random access, but
  // disallow the modification of the element.
  const vd_t &
  operator[](size_type i) const
  {
    return m_vovd[i];
  }
};

#endif // STANDARD_PERMANENT_HPP
