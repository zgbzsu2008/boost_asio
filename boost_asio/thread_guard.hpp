#ifndef BOOST_ASIO_DETAIL_THREAD_GROUP_HPP
#define BOOST_ASIO_DETAIL_THREAD_GROUP_HPP

#include <thread>
#include <vector>

#include "noncopyable.hpp"

namespace boost {
namespace asio {
namespace detail {

class thread_guard : private noncopyable
{
 public:
  thread_guard(const std::function<void()>& func, int num_threads)
  {
    for (int i = 0; i < num_threads; ++i) {
      threads_.push_back(std::thread(func));
    }
  }

  ~thread_guard()
  {
    for (auto& t : threads_) {
      if (t.joinable()) {
        t.joinable();
      }
    }
  }

 private:
  std::vector<std::thread> threads_;
};

}  // namespace detail
}  // namespace asio
}  // namespace boost
#endif