#include "epoll_reactor.hpp"

#include <string.h>
#include <sys/epoll.h>
#include <mutex>
#include <unistd.h>

namespace boost {
namespace asio {
namespace detail {
epoll_reactor::epoll_reactor(execution_context& ctx)
    : execution_context_service_base(ctx),
      scheduler_(use_service<scheduler>(ctx)),
      interrupter_(),
      epoll_fd_(do_epoll_create()),
      shutdown_(false),
      mutex_()
{
  epoll_event ev = {0, {0}};
  ev.events = EPOLLIN | EPOLLERR | EPOLLET;
  ev.data.ptr = &interrupter_;
  ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, interrupter_.fd(), &ev);
  interrupter_.interrupt();
}

epoll_reactor::~epoll_reactor()
{
  if (epoll_fd_ != -1) {
    ::close(epoll_fd_);
  }
}

void epoll_reactor::shutdown()
{
  std::unique_lock<std::mutex> lock(mutex_);
  shutdown_ = true;
  lock.unlock();
  op_queue<operation> ops;
  for (auto it = registered_descriptors_.begin();
       it != registered_descriptors_.end(); ++it) {
    for (int i = 0; i < max_ops; ++i) {
      ops.push(it->second->op_queue_[i]);
    }
    it->second->shutdown_ = true;
    delete it->second;
  }
}

void epoll_reactor::init_task() { scheduler_.init_task(); }

void epoll_reactor::run(long usec, op_queue<operation>& ops)
{
  int timeout = 0;
  if (usec) {
    timeout = int((usec < 0) ? -1 : ((usec - 1) / 1000 + 1));
  }

  epoll_event events[128];
  int numEvents = ::epoll_wait(epoll_fd_, events, 128, timeout);

  for (int i = 0; i < numEvents; ++i) {
    void* ptr = events[i].data.ptr;
    if (ptr == &interrupter_) {
    } else {
      auto data = static_cast<descriptor_state*>(ptr);
      if (!data->is_enqueued) {
        data->set_ready_events(events[i].events);
        ops.push(data);
      } else {
        data->add_ready_events(events[i].events);
      }
    }
  }
}

void epoll_reactor::interrupt() {}

void epoll_reactor::post_immediate_completion(reactor_op* op,
                                              bool is_continuation)
{
  scheduler_.post_immediate_completion(op, is_continuation);
}

int epoll_reactor::do_epoll_create()
{
  int fd = ::epoll_create1(EPOLL_CLOEXEC);
  if (fd < 0) {
    std::cout << "EPoller::do_epoll_create(): epoll_create1 failed."
              << std::endl;
    std::error_code ec(errno, std::generic_category());
    throw ec;
  }
  return fd;
}

struct epoll_reactor::perform_io_cleanup_on_block_exit
{
  explicit perform_io_cleanup_on_block_exit(epoll_reactor* r)
      : reactor_(r), first_op_(0)
  {}
  ~perform_io_cleanup_on_block_exit()
  {
    if (first_op_) {
      if (!ops_.empty()) {
        reactor_->scheduler_.post_deferred_completions(ops_);
      }
    } else {
      reactor_->scheduler_.compensating_work_started();
    }
  }
  epoll_reactor* reactor_;
  op_queue<operation> ops_;
  operation* first_op_;
};

operation* epoll_reactor::descriptor_state::perform_io(uint32_t events)
{
  mutex_.lock();
  perform_io_cleanup_on_block_exit io_cleanup(reactor_);
  std::unique_lock<std::mutex> lock(mutex_, std::adopt_lock);  // mutex locked
  static const int flag[max_ops] = {EPOLLIN, EPOLLOUT, EPOLLPRI};
  for (int j = max_ops - 1; j >= 0; --j) {
    if (events & (flag[j] | EPOLLERR | EPOLLHUP)) {
      try_speculative_[j] = true;
      while (reactor_op* op = op_queue_[j].front()) {
        if (auto status = op->perform()) {
          op_queue_[j].pop();
          io_cleanup.ops_.push(op);
          if (status == reactor_op::done_and_exhausted) {
            try_speculative_[j] = false;
            break;
          }
        } else {
          break;
        }
      }
    }
  }

  io_cleanup.first_op_ = io_cleanup.ops_.front();
  io_cleanup.ops_.pop();
  return io_cleanup.first_op_;
}

void epoll_reactor::descriptor_state::do_complete(void* owner, operation* base,
                                                  const std::error_code& ec,
                                                  std::size_t bytes_transferred)
{
  if (owner) {
    auto data = static_cast<descriptor_state*>(base);
    uint32_t events = static_cast<uint32_t>(bytes_transferred);
    if (auto op = data->perform_io(events)) {
      op->complete(owner, ec, 0);
    }
  }
}

}  // namespace detail
}  // namespace asio
}  // namespace boost