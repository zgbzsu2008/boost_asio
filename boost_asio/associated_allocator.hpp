#ifndef BOOST_ASIO_ASSOCIATED_ALLOCATOR_HPP22
#define BOOST_ASIO_ASSOCIATED_ALLOCATOR_HPP22

#include <memory>
#include <type_traits>

namespace boost::asio {

template <class, class = std::void_t<>>
struct has_allocator_type : std::false_type {};

template <class T>
struct has_allocator_type<T, std::void_t<typename T::allocator_type>>
    : std::true_type {};

template <typename T, typename Alloc = std::allocator<void>>
using associated_allocator_t =
    typename std::conditional<has_allocator_type<T>::value,
                              typename T::allocator_type, Alloc>::type;

template <typename T, typename Alloc = std::allocator<void>>
associated_allocator_t<T, Alloc> get_associated_allocator(
    const T& t, const Alloc& a = Alloc()) {
  return has_allocator_type<T>::value ? t.get_allocator() : a;
}
}  // namespace boost::asio
#endif