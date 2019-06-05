#include "scheduler.hpp"

#include <iostream>
#include <limits>

#include "epoll_reactor.hpp"
#include "scheduler_thread_info.hpp"
#include "service_registry_helpers.hpp"

namespace boost::asio::detail {
struct scheduler::task_cleanup
{
  ~task_cleanup()
  {
    if (this_thread_->private_outstanding_work > 0) {
      scheduler_->outstanding_work_ += this_thread_->private_outstanding_work;
    }
    this_thread_->private_outstanding_work = 0;

    lock_->lock();
    scheduler_->op_queue_.push(this_thread_->private_op_queue);
    scheduler_->task_interrupted_ = true;
    scheduler_->op_queue_.push(&scheduler_->task_operation_);
  }

  scheduler* scheduler_;
  std::unique_lock<std::mutex>* lock_;
  thread_info* this_thread_;
};

struct scheduler::work_cleanup
{
  ~work_cleanup()
  {
    if (this_thread_->private_outstanding_work > 0) {
      scheduler_->outstanding_work_ += this_thread_->private_outstanding_work;
    }
    this_thread_->private_outstanding_work = 0;
    scheduler_->work_finished();

    if (!this_thread_->private_op_queue.empty()) {
      lock_->lock();
      scheduler_->op_queue_.push(this_thread_->private_op_queue);
    }
  }

  scheduler* scheduler_;
  std::unique_lock<std::mutex>* lock_;
  thread_info* this_thread_;
};

scheduler::scheduler(execution_context& ctx, int concurrency_hint)
    : execution_context_service_base(ctx),
      one_thread_(concurrency_hint == 1),
      concurrency_hint_(concurrency_hint)
{}

scheduler::~scheduler() {}

void scheduler::shutdown()
{
  std::unique_lock<std::mutex> lock(mutex_);
  shutdown_ = true;
  lock.unlock();

  while (operation* o = op_queue_.front()) {
    op_queue_.pop();
    if (o != &task_operation_) {
      o->destroy();
    }
  }
  task_ = 0;
}

void scheduler::init_task()
{
  std::unique_lock<std::mutex> lock(mutex_);
  if (!shutdown_ && !task_) {
    task_ = &use_service<epoll_reactor>(this->context());
    op_queue_.push(&task_operation_);
    wake_one_thread_and_unlock(lock);
  }
}

std::size_t scheduler::run(std::error_code& ec)
{
  ec = std::error_code();
  if (outstanding_work_ == 0) {
    stop();
    return 0;
  }

  thread_info this_thread;
  this_thread.private_outstanding_work = 0;
  thread_call_stack::context ctx(this, this_thread);
  std::unique_lock<std::mutex> lock(mutex_);

  std::size_t n = 0;
  for (; do_run_one(lock, this_thread, ec); lock.lock()) {
    if (n != (std::numeric_limits<std::size_t>::max)()) {
      ++n;
    }
  }
  return n;
}

std::size_t scheduler::do_run_one(std::unique_lock<std::mutex>& lock, thread_info& this_thread,
                                  const std::error_code& ec)
{
  while (!stopped_) {
    if (!op_queue_.empty()) {
      std::cout << "scheduler::do_run_one(): working... pid= " << std::this_thread::get_id()
                << std::endl;
      operation* o = op_queue_.front();
      op_queue_.pop();
      bool more_handlers = (!op_queue_.empty());
      if (o == &task_operation_) {
        task_interrupted_ = more_handlers;
        if (more_handlers && !one_thread_) {
          wakeup_event_.unlock_and_signal_one(lock);
        } else {
          lock.unlock();
        }
        task_cleanup on_exit = {this, &lock, &this_thread};
        (void)on_exit;

        task_->run(more_handlers ? 0 : -1, this_thread.private_op_queue);
      } else {
        if (more_handlers && !one_thread_) {
          wakeup_event_.unlock_and_signal_one(lock);
        } else {
          lock.unlock();
        }
        work_cleanup on_exit = {this, &lock, &this_thread};
        (void)on_exit;

        std::size_t task_result = o->task_result_;
        o->complete(this, ec, task_result);
        return 1;
      }
    } else {
      std::cout << "scheduler::do_run_one(): waiting... pid= " << std::this_thread::get_id()
                << std::endl;
      wakeup_event_.clear(lock);
      wakeup_event_.wait(lock);
    }
  }
  return 0;
}

void scheduler::stop()
{
  std::unique_lock<std::mutex> lock(mutex_);
  this->stop_all_threads(lock);
}

bool scheduler::stopped() const
{
  std::unique_lock<std::mutex> lock(mutex_);
  return stopped_;
}

void scheduler::restart()
{
  std::unique_lock<std::mutex> lock(mutex_);
  stopped_ = false;
}

void scheduler::compensating_work_started()
{
  thread_info_base* this_thread = thread_call_stack::contains(this);
  ++(static_cast<thread_info*>(this_thread)->private_outstanding_work);
}

void scheduler::do_dispatch(operation* op)
{
  work_started();
  std::unique_lock<std::mutex> lock(mutex_);
  op_queue_.push(op);
  wake_one_thread_and_unlock(lock);
}

void scheduler::post_immediate_completion(operation* op, bool is_continuation)
{
  if (one_thread_ || is_continuation) {
    if (thread_info_base* this_thread = thread_call_stack::contains(this)) {
      ++(static_cast<thread_info*>(this_thread)->private_outstanding_work);
      (static_cast<thread_info*>(this_thread))->private_op_queue.push(op);
      return;
    }
  }

  work_started();
  std::unique_lock<std::mutex> lock(mutex_);
  op_queue_.push(op);
  wake_one_thread_and_unlock(lock);
}

void scheduler::post_deferred_completion(operation* op)
{
  if (one_thread_) {
    if (thread_info_base* this_thread = thread_call_stack::contains(this)) {
      // ++(static_cast<thread_info*>(this_thread)->private_outstanding_work);
      (static_cast<thread_info*>(this_thread))->private_op_queue.push(op);
      return;
    }
  }

  // work_started();
  std::unique_lock<std::mutex> lock(mutex_);
  op_queue_.push(op);
  wake_one_thread_and_unlock(lock);
}

void scheduler::post_deferred_completions(op_queue<operation>& ops)
{
  if (one_thread_) {
    if (thread_info_base* this_thread = thread_call_stack::contains(this)) {
      ops.push(static_cast<thread_info*>(this_thread)->private_op_queue);
      return;
    }
  }

  std::unique_lock<std::mutex> lock(mutex_);
  op_queue_.push(ops);
  wake_one_thread_and_unlock(lock);
}

void scheduler::abandon_operations(op_queue<operation>& ops)
{
  op_queue<operation> ops2;
  ops2.push(ops);
}

void scheduler::stop_all_threads(std::unique_lock<std::mutex>& lock)
{
  stopped_ = true;
  wakeup_event_.signal_all(lock);
  if (!task_interrupted_ && task_) {
    task_interrupted_ = true;
    task_->interrupt();
  }
}

void scheduler::wake_one_thread_and_unlock(std::unique_lock<std::mutex>& lock)
{
  if (!wakeup_event_.maybe_unlock_and_signal_one(lock)) {
    if (&task_interrupted_ && task_) {
      task_interrupted_ = true;
      task_->interrupt();
    }
    lock.unlock();
  }
}
}  // namespace boost::asio::detail
