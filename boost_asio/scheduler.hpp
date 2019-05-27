#ifndef BOOST_ASIO_DETAIL_SCHEDULER_HPP
#define BOOST_ASIO_DETAIL_SCHEDULER_HPP

#include "thread_context.hpp"

namespace boost {
namespace asio {
namespace detail {
class scheduler : public thread_context {
 public:
  using thread_info = 
  std::size_t run(std::error_code& ec);
  std::size_t do_run_one(std::unique_lock<std::mutex>& lock,
                         thread_info& this_thread, const std::error_code& ec);
 private:

};
}  // namespace detail
}  // namespace asio
}  // namespace boost
#endif