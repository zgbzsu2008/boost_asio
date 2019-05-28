#ifndef BOOST_ASIO_DETAIL_EPOLL_REACTOR_HPP
#define BOOST_ASIO_DETAIL_EPOLL_REACTOR_HPP

#include <mutex>
#include <map>

#include "execution_context.hpp"
#include "io_context.hpp"
#include "reactor_op.hpp"
#include "select_interrupter.hpp"

struct epoll_event;

namespace boost {
namespace asio {
namespace detail {

class epoll_reactor : public execution_context_service_base<epoll_reactor>
{
 public:
  enum op_types
  {
    read_op = 0,
    write_op = 1,
    connect_op = 1,
    except_op = 2,
    max_ops = 3
  };

  class descriptor_state : operation
  {
    friend class epoll_reactor;

    mutable std::mutex mutex_;
    epoll_reactor* reactor_;
    int descriptor_;
    uint32_t registered_events_;
    bool is_enqueued;
    op_queue<reactor_op> op_queue_[max_ops];
    bool try_speculative_[max_ops];
    bool shutdown_;

    descriptor_state(bool locking);
    void set_ready_events(uint32_t events) { task_result_ = events; }
    void add_ready_events(uint32_t events) { task_result_ |= events; }
    operation* perform_io(uint32_t events);

    static void do_complete(void* owner, operation* base,
                            const std::error_code& ec,
                            std::size_t bytes_transferred);
  };

  epoll_reactor(execution_context& ctx);
  ~epoll_reactor();

  void shutdown();
  void init_task();

  void run(long usec, op_queue<operation>& ops);
  void interrupt();
  void post_immediate_completion(reactor_op* op, bool is_continuation);

 private:
  static int do_epoll_create();

  scheduler& scheduler_;
  select_interrupter interrupter_;
  int epoll_fd_;
  bool shutdown_;
  mutable std::mutex mutex_;
  std::map<int, descriptor_state*> registered_descriptors_;
  struct perform_io_cleanup_on_block_exit;
  friend struct perform_io_cleanup_on_block_exit;
};

}  // namespace detail
}  // namespace asio
}  // namespace boost
#endif
