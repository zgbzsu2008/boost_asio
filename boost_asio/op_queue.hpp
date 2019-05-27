#ifndef BOOST_ASIO_DETAIL_OP_QUEUE_HPP
#define BOOST_ASIO_DETAIL_OP_QUEUE_HPP

#include <list>
#include <queue>

namespace boost {
namespace asio {
namespace detail {

template <typename T>
class op_queue : public std::queue<T*, std::list<T*>> {
 public:
  template <typename U>
  void push(op_queue<U>& q) { c.splice(c.begin(), q.c); }

  T* front() {
    return empty() ? std::queue<T*, std::list<T*>>::front()
                   : static_cast<T*>(0);
  }
};

}  // namespace detail
}  // namespace asio
}  // namespace boost
#endif