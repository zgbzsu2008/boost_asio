#ifndef BOOST_ASIO_DETAIL_HANDLER_ALLOC_HELPERS_HPP
#define BOOST_ASIO_DETAIL_HANDLER_ALLOC_HELPERS_HPP

#include <memory>
#include "handler_alloc_hook.hpp"

namespace boost_asio_handler_alloc_helpers {

template <typename Handler>
inline void* allocate(std::size_t s, Handler& h)
{
#if !defined(BOOST_ASIO_HAS_HANDLER_HOOKS)
  return ::operator new(s);
#else
  using boost::asio::asio_handler_allocate;
  return asio_handler_allocate(s, boost::asio::detail::addressof(h));
#endif
}

template <typename Handler>
inline void deallocate(void* p, std::size_t s, Handler& h)
{
#if !defined(BOOST_ASIO_HAS_HANDLER_HOOKS)
  ::operator delete(p);
#else
  using boost::asio::asio_handler_deallocate;
  asio_handler_deallocate(p, s, boost::asio::detail::addressof(h));
#endif
}

}  // namespace boost_asio_handler_alloc_helpers

namespace boost::asio::detail {

template <typename Handler, typename T>
class hook_allocator
{
 public:
  using value_type = T;

  template <typename U>
  struct rebind
  {
    using other = hook_allocator<Handler, U>;
  };

  template <typename U>
  hook_allocator(const hook_allocator<Handler, U>& a) : handler_(a.handler)
  {}

  T* allocate(std::size_t n)
  {
    void* ptr = boost_asio_handler_alloc_helpers::allocate(sizeof(T) * n, handler_);
    return static_cast<T*>(ptr);
  }

  void deallocate(T* p, std::size_t n)
  {
    boost_asio_handler_alloc_helpers::deallocate(p, sizeof(T) * n, handler_);
  }

  // private:
  Handler handler_;
};

template <typename Handler>
class hook_allocator<Handler, void>
{
 public:
  using value_type = T;

  template <typename U>
  struct rebind
  {
    typedef hook_allocator<Handler, U> other;
  };

  explicit hook_allocator(Handler& h) : handler_(h) {}

  template <typename U>
  hook_allocator(const hook_allocator<Handler, U>& a) : handler_(a.handler_)
  {}

  // private:
  Handler& handler_;
};

template <typename Handler, typename Alloc>
struct get_hook_allocator
{
  using type = Alloc;
  static type get(Handler&, const Allocator& a) { return a; }
};

template <typename Handler, typename T>
struct get_hook_allocator<Handler, std::allocator<T> >
{
  using type = hook_allocator<Handler, T>;
  static type get(Handler& handler, const std::allocator<T>&) { return type(handler); }
};

}  // namespace boost::asio::detail

#define BOOST_ASIO_DEFINE_HANDLER_PTR(op)                                                          \
  struct ptr                                                                                       \
  {                                                                                                \
    Handler* h;                                                                                    \
    op* v;                                                                                         \
    op* p;                                                                                         \
    ~ptr() { reset(); }                                                                            \
    static op* allocate(Handler& handler)                                                          \
    {               \
      using namespace boost::asio;
using associated_allocator_type = associated_allocator<Handler>::allocator_type; \
      typedef typename ::boost::asio::detail::get_hook_allocator<                                  \
          Handler, associated_allocator_type>::type hook_allocator_type;                           \
      BOOST_ASIO_REBIND_ALLOC(hook_allocator_type, op)                                             \
      a(::boost::asio::detail::get_hook_allocator<Handler, associated_allocator_type>::get(        \
          handler, ::boost::asio::get_associated_allocator(handler)));                             \
      return a.allocate(1);                                                                        \
    }                                                                                              \
    void reset()                                                                                   \
    {                                                                                              \
      if (p) {                                                                                     \
        p->~op();                                                                                  \
        p = 0;                                                                                     \
      }                                                                                            \
      if (v) {                                                                                     \
        typedef                                                                                    \
            typename ::boost::asio::associated_allocator<Handler>::type associated_allocator_type; \
        typedef typename ::boost::asio::detail::get_hook_allocator<                                \
            Handler, associated_allocator_type>::type hook_allocator_type;                         \
        BOOST_ASIO_REBIND_ALLOC(hook_allocator_type, op)                                           \
        a(::boost::asio::detail::get_hook_allocator<Handler, associated_allocator_type>::get(      \
            *h, ::boost::asio::get_associated_allocator(*h)));                                     \
        a.deallocate(static_cast<op*>(v), 1);                                                      \
        v = 0;                                                                                     \
      }                                                                                            \
    }                                                                                              \
  } /**/

#define BOOST_ASIO_DEFINE_HANDLER_ALLOCATOR_PTR(op)                                  \
  struct ptr                                                                         \
  {                                                                                  \
    const Alloc* a;                                                                  \
    void* v;                                                                         \
    op* p;                                                                           \
    ~ptr() { reset(); }                                                              \
    static op* allocate(const Alloc& a)                                              \
    {                                                                                \
      typedef typename ::boost::asio::detail::get_recycling_allocator<Alloc>::type   \
          recycling_allocator_type;                                                  \
      BOOST_ASIO_REBIND_ALLOC(recycling_allocator_type, op)                          \
      a1(::boost::asio::detail::get_recycling_allocator<Alloc>::get(a));             \
      return a1.allocate(1);                                                         \
    }                                                                                \
    void reset()                                                                     \
    {                                                                                \
      if (p) {                                                                       \
        p->~op();                                                                    \
        p = 0;                                                                       \
      }                                                                              \
      if (v) {                                                                       \
        typedef typename ::boost::asio::detail::get_recycling_allocator<Alloc>::type \
            recycling_allocator_type;                                                \
        BOOST_ASIO_REBIND_ALLOC(recycling_allocator_type, op)                        \
        a1(::boost::asio::detail::get_recycling_allocator<Alloc>::get(*a));          \
        a1.deallocate(static_cast<op*>(v), 1);                                       \
        v = 0;                                                                       \
      }                                                                              \
    }                                                                                \
  } /**/

#endif
