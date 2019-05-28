#ifndef BOOST_ASIO_DETAIL_OP_QUEUE_HPP
#define BOOST_ASIO_DETAIL_OP_QUEUE_HPP

#include <iostream>
#include <list>
#include <queue>

namespace boost {
namespace asio {
namespace detail {

template <typename T>
class op_queue : public std::queue<T*, std::list<T*>>
{
 public:
  using std::queue<T*, std::list<T*>>::push;

  void push(op_queue& q) { this->c.splice(this->c.end(), q.c); }

  template <typename U>
  void push(op_queue<U>& q)
  {
    while (auto v = q.front()) {
      this->push(static_cast<T*>(v));
    }
  }

  T* front()
  {
    return this->empty() ? std::queue<T*, std::list<T*>>::front() : 0;
  }

  T* back()
  {
    return this->empty() ? std::queue<T*, std::list<T*>>::back() : 0;
  }
};

}  // namespace detail
}  // namespace asio
}  // namespace boost
#endif