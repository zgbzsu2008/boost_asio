#ifndef BOOST_ASIO_DETAIL_SCHEDULER_HPP
#define BOOST_ASIO_DETAIL_SCHEDULER_HPP

#include <atomic>
#include <mutex>

#include "event.hpp"
#include "execution_context.hpp"
#include "scheduler_operation.hpp"
#include "scheduler_thread_info.hpp"
#include "thread_context.hpp"

namespace boost {
namespace asio {
namespace detail {

struct scheduler_thread_info;
class epoll_reactor;

class scheduler : public execution_context_service_base<scheduler>,
                  public thread_context
{
 public:
  using thread_info = scheduler_thread_info;

  scheduler(execution_context& ctx, int concurrency_hint = 1);
  ~scheduler();

  void shutdown();
  void init_task();

  std::size_t run(std::error_code& ec);
  std::size_t do_run_one(std::unique_lock<std::mutex>& lock,
                         thread_info& this_thread, const std::error_code& ec);
  void stop();
  bool stopped() const;
  void restart();

  void work_started() { ++outstanding_work_; }
  void compensating_work_started();
  void work_finished()
  {
    if (--outstanding_work_ == 0) {
      stop();
    }
  }

  bool can_dispatch() { return thread_call_stack::contains(this) != 0; }
  void do_dispatch(operation* op);

  void post_immediate_completion(operation* op, bool is_continuation);
  void post_deferred_completion(operation* op);
  void post_deferred_completions(op_queue<operation>& ops);
  void abandon_operations(op_queue<operation>& ops);

 private:
  void stop_all_threads(std::unique_lock<std::mutex>& lock);
  void wake_one_thread_and_unlock(std::unique_lock<std::mutex>& lock);

  struct task_cleanup;
  friend struct task_cleanup;

  struct work_cleanup;
  friend struct work_cleanup;

  const bool one_thread_;
  mutable std::mutex mutex_;
  event wakeup_event_;

  epoll_reactor* task_;

  struct task_operation : operation
  {
    task_operation() : operation(0) {}
  } task_operation_;

  bool task_interrupted_;
  std::atomic<std::size_t> outstanding_work_;
  op_queue<operation> op_queue_;

  bool stopped_;
  bool shutdown_;

  const int concurrency_hint_;
};
}  // namespace detail
}  // namespace asio
}  // namespace boost
#endif