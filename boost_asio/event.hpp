#ifndef BOOST_ASIO_DETAIL_EVENT_HPP
#define BOOST_ASIO_DETAIL_EVENT_HPP

#include <assert.h>

#include <chrono>
#include <condition_variable>
#include <thread>

#include "noncopyable.hpp"

namespace boost::asio::detail {
class event : private noncopyable
{
 public:
  void signal_all(std::unique_lock<std::mutex>& lock)
  {
    assert(lock.owns_lock());
    (void)lock;
    state_ |= 1;
    cond_.notify_all();
  }

  void signal(std::unique_lock<std::mutex>& lock) { this->signal_all(lock); }

  void unlock_and_signal_one(std::unique_lock<std::mutex>& lock)
  {
    assert(lock.owns_lock());
    state_ |= 1;
    bool have_waiters = (state_ > 1);
    lock.unlock();
    if (have_waiters) {
      cond_.notify_one();
    }
  }

  bool maybe_unlock_and_signal_one(std::unique_lock<std::mutex>& lock)
  {
    assert(lock.owns_lock());
    state_ |= 1;
    bool have_waiters = (state_ > 1);
    if (have_waiters) {
      lock.unlock();
      cond_.notify_one();
      return true;
    }
    return false;
  }

  void clear(std::unique_lock<std::mutex>& lock)
  {
    assert(lock.owns_lock());
    (void)lock;
    state_ &= ~std::size_t(1);
  }

  void wait(std::unique_lock<std::mutex>& lock)
  {
    assert(lock.owns_lock());
    while ((state_ & 1) == 0) {
      waiter w(state_);
      cond_.wait(lock);
    }
  }

  bool wait_for_usec(std::unique_lock<std::mutex>& lock, long usec)
  {
    assert(lock.owns_lock());
    while ((state_ & 1) == 0) {
      waiter w(state_);
      cond_.wait_for(lock, std::chrono::microseconds(usec));
    }
    return (state_ & 1) != 0;
  }

 private:
  class waiter
  {
   public:
    explicit waiter(std::size_t& state) : state_(state) { state_ += 2; }
    ~waiter() { state_ -= 2; }

   private:
    std::size_t& state_;
  };

  std::size_t state_ = 0;
  std::condition_variable cond_;
};
}  // namespace boost::asio::detail
#endif
