#ifndef BOOST_ASIO_SYSTEM_EXECUTOR_HPP
#define BOOST_ASIO_SYSTEM_EXECUTOR_HPP

#include <functional>
#include <memory>
#include <type_traits>

#include "executor_op.hpp"
#include "global.hpp"
#include "system_context.hpp"

namespace boost::asio {
class system_executor {
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
inline void system_executor::dispatch(Function&& func, const Alloc& a) const {
  using func_type = typename std::decay<Function>::type;
  func_type copy_func(func);
  copy_func();
}

template <typename Function, typename Alloc>
inline void system_executor::post(Function&& func, const Alloc& a) const {
  using func_type = typename std::decay<Function>::type;
  system_context& ctx = detail::global<system_context>();

  using op = detail::executor_op<func_type, Alloc>;
  typename op::ptr p = {std::addressof(a), op::ptr::allocate(a), 0};
  p.p = new (p.v) op(std::forward<Function>(func), a);

  ctx.scheduler_.post_immediate_completion(p.p, false);
  p.v = p.p = 0;
}

template <typename Function, typename Alloc>
inline void system_executor::defer(Function&& func, const Alloc& a) const {
  using func_type = typename std::decay<Function>::type;
  system_context& ctx = detail::global<system_context>();

  using op = detail::executor_op<func_type, Alloc>;
  typename op::ptr p = {std::addressof(a), op::ptr::allocate(a), 0};
  p.p = new (p.v) op(std::forward<Function>(func), a);

  ctx.scheduler_.post_immediate_completion(p.p, true);
  p.v = p.p = 0;
}
}  // namespace boost::asio
#endif