#ifndef BOOST_ASIO_DETAIL_EPOLL_REACTOR_HPP
#define BOOST_ASIO_DETAIL_EPOLL_REACTOR_HPP

#include <mutex>

#include "execution_context.hpp"
#include "scheduler_operation.hpp"
#include "select_interrupter.hpp"
#include "io_context.hpp"

namespace boost {
namespace asio {
namespace detail {

class epoll_reactor : public execution_context_service_base<epoll_reactor>
{
 public:
  epoll_reactor(execution_context& ctx);
  ~epoll_reactor();

  void shutdown();

  void run(long usec, op_queue<operation>& ops);

  scheduler& scheduler_;
  select_interrupter interrupter_;

  int epoll_fd_;

  bool shutdown_;
  mutable std::mutex mutex_;
  mutable std::mutex registered_descriptors_mutex_;
};

}  // namespace detail
}  // namespace asio
}  // namespace boost
#endif
