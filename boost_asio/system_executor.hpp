#ifndef BOOST_ASIO_SYSTEM_EXECUTOR_HPP
#define BOOST_ASIO_SYSTEM_EXECUTOR_HPP

#include <functional>
#include <memory>
#include <type_traits>

#include "executor_op.hpp"
#include "global.hpp"
#include "handler_invoke_helpers.hpp"
#include "system_context.hpp"

namespace boost::asio {
class system_executor
{
 public:
  system_context& context() const { return detail::global<system_context>(); }

  void on_work_started() const {}
  void on_work_finished() const {}

  template <typename Function, typename Alloc>
  void dispatch(Function&& func, const Alloc& a) const;

  template <typename Function, typename Alloc>
  void post(Function&& func, const Alloc& a) const;

  template <typename Function, typename Alloc>
  void defer(Function&& func, const Alloc& a) const;
};

template <typename Function, typename Alloc>
inline void system_executor::dispatch(Function&& func, const Alloc& a) const
{
  typename std::decay<Function>::type tmp(std::move(func));
  boost_asio_handler_invoke_helpers::invoke(tmp, tmp);
}

template <typename Function, typename Alloc>
inline void system_executor::post(Function&& func, const Alloc& a) const
{
  using op = detail::executor_op<typename std::decay<Function>::type, Alloc>;
  typename op::ptr p = {std::addressof(a), op::ptr::allocate(a), 0};
  p.p = new (p.v) op(std::move(func), a);

  system_context& ctx = detail::global<system_context>();
  ctx.scheduler_.post_immediate_completion(p.p, false);
  p.v = p.p = 0;
}

template <typename Function, typename Alloc>
inline void system_executor::defer(Function&& func, const Alloc& a) const
{
  using op = detail::executor_op<typename std::decay<Function>::type, Alloc>;
  typename op::ptr p = {std::addressof(a), op::ptr::allocate(a), 0};
  p.p = new (p.v) op(std::move(func), a);

  system_context& ctx = detail::global<system_context>();
  ctx.scheduler_.post_immediate_completion(p.p, true);
  p.v = p.p = 0;
}
}  // namespace boost::asio
#endif