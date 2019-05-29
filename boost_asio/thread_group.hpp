#ifndef BOOST_ASIO_DETAIL_THREAD_GROUP_HPP
#define BOOST_ASIO_DETAIL_THREAD_GROUP_HPP

#include <functional>
#include <thread>
#include <vector>

#include "noncopyable.hpp"

namespace boost::asio::detail {
class thread_group : private noncopyable {
 public:
  thread_group() {}
  thread_group(const std::function<void()>& func, int num_threads) {
    create_threads(func, num_threads);
  }

  ~thread_group() { join(); }

  void create_threads(const std::function<void()>& func, int num_threads) {
    for (int i = 0; i < num_threads; ++i) {
      threads_.push_back(std::thread(func));
    }
  }

  void join() {
    for (auto& t : threads_) {
      if (t.joinable()) {
        t.joinable();
      }
    }
  }

 private:
  std::vector<std::thread> threads_;
};
}  // namespace boost::asio::detail
#endif