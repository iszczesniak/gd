#ifndef ACCOUNTANT_HPP
#define ACCOUNTANT_HPP

// This class keeps track of an integer number, and remembers its
// maximum.  The accountant is shared between the permanent and
// tentative solutions to find the highest memory usage.
template <typename T>
struct accountant
{
  T m_max = 0;
  T m_cur = 0;

  accountant &
  operator++()
  {
    ++m_cur;
    if (m_max < m_cur)
      m_max = m_cur;
    return *this;
  }

  accountant &
  operator--()
  {
    --m_cur;
    return *this;
  }
};

#endif // ACCOUNTANT_HPP
