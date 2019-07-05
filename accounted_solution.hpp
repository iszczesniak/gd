#ifndef ACCOUNTED_SOLUTION_HPP
#define ACCOUNTED_SOLUTION_HPP

#include <utility>

// The accounted container type for storing both permanent and
// tentative labels.
template <typename Solution, typename Accountant>
struct accounted_solution: public Solution
{
  using label_t = typename Solution::label_t;
  using size_type = typename Solution::size_type;

  Accountant &m_acc;

  accounted_solution(Accountant &acc, size_type count):
    m_acc(acc), Solution(count)
  {
  }

  template <typename T>
  void
  push(T &&l)
  {
    ++m_acc;
    Solution::push(std::forward<T>(l));
  }

  label_t
  pop()
  {
    --m_acc;
    return Solution::pop();
  }
};

#endif // ACCOUNTED_SOLUTION_HPP
