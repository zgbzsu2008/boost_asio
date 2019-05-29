#ifndef BOOST_ASIO_ASSOCIATED_EXECUTOR_HPP
#define BOOST_ASIO_ASSOCIATED_EXECUTOR_HPP

#include "is_executor.hpp"
#include "system_executor.hpp"

namespace boost::asio {
template <typename, typename = std::void_t<>>
struct has_executor_type : std::false_type
{
  using executor_type = system_executor;
};

template <typename T>
struct has_executor_type<T, std::void_t<typename T::executor_type>> : std::true_type
{
  using executor_type = typename T::executor_type;
};

template <typename T, typename Executor = system_executor>
auto get_associated_executor(const T& t, const Executor& ex = Executor())
{
  if constexpr (has_executor_type<T>::value) {
    static_assert(std::is_convertible<T&, execution_context&>::value);
    static_assert(std::is_member_function_pointer<decltype(&T::get_executor)>::value);
    static_assert(detail::is_executor<typename T::executor_type>::value);
    return const_cast<T*>(&t)->get_executor();
  } else {
    static_assert(detail::is_executor<Executor>::value);
    return ex;
  }
}

template <typename T, typename Executor = system_executor>
using associated_executor_t =
    typename std::conditional<has_executor_type<T>::value,
                              typename has_executor_type<T>::executor_type, Executor>::type;
}  // namespace boost::asio
#endif