#ifndef BOOST_ASIO_DETAIL_IS_EXECUTOR_HPP
#define BOOST_ASIO_DETAIL_IS_EXECUTOR_HPP

#include <type_traits>

namespace boost::asio::detail
{
template <typename, typename = std::void_t<>, typename = std::void_t<>,
          typename = std::void_t<>, typename = std::void_t<>,
          typename = std::void_t<>, typename = std::void_t<>>
struct is_executor_class : std::false_type
{
};

template <typename T>
struct is_executor_class<
    T, std::void_t<decltype(&T::context)>,
    std::void_t<decltype(&T::on_work_started)>,
    std::void_t<decltype(&T::on_work_finished)>,
    std::void_t<decltype(
        &T::template dispatch<void (*)(), std::allocator<void>>)>,
    std::void_t<decltype(&T::template post<void (*)(), std::allocator<void>>)>,
    std::void_t<decltype(&T::template defer<void (*)(), std::allocator<void>>)>>
  : std::true_type
{
};

template <typename T>
struct is_executor
  : public std::conditional<std::is_class<T>::value, is_executor_class<T>,
                            std::false_type>::type
{
};
};  // namespace boost::asio::detail
#endif
