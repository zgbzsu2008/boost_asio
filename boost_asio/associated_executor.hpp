#ifndef BOOST_ASIO_ASSOCIATED_EXECUTOR_HPP
#define BOOST_ASIO_ASSOCIATED_EXECUTOR_HPP

#include "is_executor.hpp"
#include "system_executor.hpp"

/*
class myExecutor
class myClass
{
 public:
  using executor_type = myExecutor;
  myExecutor get_executor();
};
*/
namespace boost::asio
{
template <typename, typename Executor, typename = std::void_t<>>
struct associated_executor : std::false_type
{
  using executor_type = Executor;
};

template <typename T, typename Executor>
struct associated_executor<T, Executor, std::void_t<typename T::executor_type>> : std::true_type
{
  using executor_type = typename T::executor_type;
};

template <typename T, typename Executor = system_executor>
auto get_associated_executor(const T& t, const Executor& ex = Executor())
{
  if constexpr(associated_executor<T, Executor>::value)
  {
    static_assert(std::is_convertible<T&, execution_context&>::value);
    static_assert(std::is_member_function_pointer<decltype(&T::get_executor)>::value);
    static_assert(
      detail::is_executor<typename associated_executor<T, Executor>::executor_type>::value);
    return const_cast<T*>(&t)->get_executor();
  }
  else
  {
    static_assert(detail::is_executor<Executor>::value);
    return ex;
  }
}

template <typename T, typename Executor = system_executor>
using associated_executor_t = typename associated_executor<T, Executor>::executor_type;
}  // namespace boost::asio
#endif
