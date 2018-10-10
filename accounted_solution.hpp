#ifndef ACCOUNTED_SOLUTION_HPP
#define ACCOUNTED_SOLUTION_HPP

// The accounted container type for storing both permanent and
// tentative labels.
template <typename Solution, typename Accountant>
struct accounted_solution: public Solution
{
  using label_type = typename Solution::label_type;

  Accountant &m_acc;

  accounted_solution(Accountant &acc): m_acc(acc)
  {
  }

  void
  push(const label_type &l)
  {
    ++m_acc;
    Solution::push(l);
  }

  auto
  insert(typename Solution::node_type &&nh)
  {
    ++m_acc;
    return Solution::insert(std::move(nh));
  }

  auto
  pop()
  {
    --m_acc;
    return Solution::pop();
  }
};

#endif // ACCOUNTED_SOLUTION_HPP
