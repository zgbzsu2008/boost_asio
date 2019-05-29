#ifndef BOOST_ASIO_ASSOCIATED_ALLOCATOR_HPP
#define BOOST_ASIO_ASSOCIATED_ALLOCATOR_HPP

#include <memory>
#include <type_traits>

namespace boost::asio {

template <typename, typename = std::void_t<>>
struct has_allocator_type : std::false_type
{
  using allocator_type = std::allocator<void>;
};

template <typename T>
struct has_allocator_type<T, std::void_t<typename T::allocator_type>> : std::true_type
{
  using allocator_type = typename T::allocator_type;
};

template <typename T, typename Alloc = std::allocator<void>>
auto get_associated_allocator(const T& t, const Alloc& a = Alloc())
{
  if constexpr (has_allocator_type<T>::value) {
    static_assert(std::is_member_function_pointer<decltype(&T::get_allocator)>::value);
    return t.get_allocator();
  } else {
    return a;
  }
}

template <typename T, typename Alloc = std::allocator<void>>
using associated_allocator_t = typename has_allocator_type<T>::allocator_type;

}  // namespace boost::asio
#endif