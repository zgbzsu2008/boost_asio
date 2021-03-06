#ifndef BOOST_ASIO_ASSOCIATED_ALLOCATOR_HPP
#define BOOST_ASIO_ASSOCIATED_ALLOCATOR_HPP

#include <memory>
#include <type_traits>

/*
class myAllocator
class myClass
{
 public:
  using allocator_type = myAllocator;
  myAllocator get_allocator();
};
*/
namespace boost::asio {
template <typename, typename E, typename = std::void_t<>>
struct associated_allocator : std::false_type
{
  using allocator_type = E;
};

template <typename T, typename E>
struct associated_allocator<T, E, std::void_t<typename T::allocator_type>> : std::true_type
{
  using allocator_type = typename T::allocator_type;
};

template <typename T, typename Alloc = std::allocator<void>>
auto get_associated_allocator(const T& t, const Alloc& a = Alloc())
{
  if constexpr (associated_allocator<T, Alloc>::value) {
    static_assert(std::is_member_function_pointer<decltype(&T::get_allocator)>::value);
    return t.get_allocator();
  } else {
    return a;
  }
}

template <typename T, typename Alloc = std::allocator<void>>
using associated_allocator_t = typename associated_allocator<T, Alloc>::allocator_type;

}  // namespace boost::asio
#endif
