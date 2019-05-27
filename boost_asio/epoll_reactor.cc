#include "epoll_reactor.hpp"

#include <sys/epoll.h>
#include <mutex>

#include "service_registry.hpp"
#include "io_context.hpp"

namespace boost {
namespace asio {
namespace detail {
epoll_reactor::epoll_reactor(execution_context& ctx)
    : execution_context_service_base(ctx),
      scheduler_(use_service<scheduler>(ctx))
{
}

epoll_reactor::~epoll_reactor() {}

void epoll_reactor::shutdown() {}

void epoll_reactor::run(long usec, op_queue<operation>& ops)
{
  int timeout;
  if (usec == 0) {
    timeout = 0;
  } else {
    timeout = int((usec < 0) ? -1 : ((usec - 1) / 1000 + 1));
  }

  epoll_event events[128];
  int numEvents = ::epoll_wait(epoll_fd_, events, 128, timeout);
  (void)numEvents;
}

}  // namespace detail
}  // namespace asio
}  // namespace boost