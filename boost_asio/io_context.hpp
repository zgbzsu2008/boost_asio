#ifndef BOOST_IO_CONTEXT_HPP
#define BOOST_IO_CONTEXT_HPP

#include <mutex>
#include <system_error>

#include "execution_context.hpp"
#include "executor_op.hpp"
#include "fenced_block.hpp"
#include "scheduler.hpp"
#include "scheduler_operation.hpp"

namespace boost::asio
{
class io_context : public execution_context
{
 public:
  using impl_type = detail::scheduler;
  class service;

  class executor_type;
  friend class executor_type;

  io_context();
  explicit io_context(int concurrency_hint);
  ~io_context();

  executor_type get_executor();

  size_t run();

 private:
  template <typename Service>
  friend Service& use_service(io_context& ioc);

  impl_type& add_impl(impl_type* impl);
  impl_type& impl_;
};

class io_context::executor_type
{
 public:
  io_context& context() const { return io_context_; }

  void on_work_started() const { io_context_.impl_.work_started(); }
  void on_work_finished() const { io_context_.impl_.work_finished(); };

  template <typename Function, typename Alloc>
  void dispatch(Function&& func, const Alloc& a) const;

  template <typename Function, typename Alloc>
  void post(Function&& func, const Alloc& a) const;

  template <typename Function, typename Alloc>
  void defer(Function&& func, const Alloc& a) const;

  bool running_in_this_thread() const { return io_context_.impl_.can_dispatch(); }

 private:
  friend class io_context;
  explicit executor_type(io_context& ioc) : io_context_(ioc) {}

  io_context& io_context_;
};

class io_context::service : public execution_context::service
{
 public:
  service(io_context& owner) : execution_context::service(owner) {}
  virtual ~service() {}

  static std::string key() { return "io_context::service"; }

  io_context& get_io_context() { return static_cast<io_context&>(context()); }

 private:
  virtual void shutdown() {}
};

namespace detail
{
template <typename Type>
class service_base : public io_context::service
{
 public:
  service_base(io_context& io) : io_context::service(io) {}

  static std::string key() { return typeid(Type).name(); }
};
}  // namespace detail

template <typename Function, typename Alloc>
inline void io_context::executor_type::dispatch(Function&& func, const Alloc& a) const
{
  if(running_in_this_thread())
  {
    detail::fenced_block b(detail::fenced_block::full);
    func();
    return;
  }

  using op = detail::executor_op<Function, Alloc>;
  typename op::ptr p = {std::addressof(a), op::ptr::allocate(a), 0};

  p.p = new(p.v) op(std::move(func), a);
  io_context_.impl_.post_immediate_completion(p.p, false);
  p.v = p.p = 0;
}

template <typename Function, typename Alloc>
inline void io_context::executor_type::post(Function&& func, const Alloc& a) const
{
  using op = detail::executor_op<Function, Alloc>;
  typename op::ptr p = {std::addressof(a), op::ptr::allocate(a), 0};

  p.p = new(p.v) op(std::move(func), a);
  io_context_.impl_.post_immediate_completion(p.p, false);
  p.v = p.p = 0;
}

template <typename Function, typename Alloc>
inline void io_context::executor_type::defer(Function&& func, const Alloc& a) const
{
  using op = detail::executor_op<Function, Alloc, detail::operation>;
  typename op::ptr p = {std::addressof(a), op::ptr::allocate(a), 0};

  p.p = new(p.v) op(std::move(func), a);
  io_context_.impl_.post_immediate_completion(p.p, true);
  p.v = p.p = 0;
}
}  // namespace boost::asio
#endif