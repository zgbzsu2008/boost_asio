#ifndef BOOST_ASIO_ASSOCIATED_EXECUTOR_HPP
#define BOOST_ASIO_ASSOCIATED_EXECUTOR_HPP

#include <type_traits>
#include "is_executor.hpp"
#include "system_context.hpp"

namespace boost::asio {
template <class, class = std::void_t<>>
struct has_executor_type : std::false_type {};

template <class T>
struct has_executor_type<T, std::void_t<typename T::executor_type>>
    : std::true_type {};

template <typename T, typename Executor = system_executor>
using associated_executor_t =
    typename std::conditional<has_executor_type<T>::value,
                              typename T::executor_type, Executor>::type;

template <typename T, typename Executor = system_executor>
associated_executor_t<T, Executor> get_associated_executor(
    const T& t, const Executor& ex = Executor()) {
  return has_executor_type<T>::value ? const_cast<T*>(&t)->get_executor() : ex;
}

template <typename T, typename Executor = system_executor>
using associated_executor_t =
    typename std::conditional<has_executor_type<T>::value,
                              typename T::executor_type, Executor>::type;
}  // namespace boost::asio
#endif
