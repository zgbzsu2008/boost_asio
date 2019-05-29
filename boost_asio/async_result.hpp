#ifndef BOOST_ASIO_ASYNC_RESULT_HPP
#define BOOST_ASIO_ASYNC_RESULT_HPP

#include <type_traits>

namespace boost::asio {

template <typename T, typename S>
class async_result {
 public:
  using handler_type = T;
  using return_type = void;

  explicit async_result(handler_type& h) { (void)h; }
  return_type get() {}

 private:
  async_result(const async_result&) = delete;
  async_result& operator=(const async_result&) = delete;
};

template <typename T, typename S>
class async_completion {
 private:
  using type =
      typename async_result<typename std::decay<T>::type, S>::handler_type;

 public:
#if defined(BOOST_ASIO_HAS_MOVE)
  using handler_type =
      typename std::conditional_t<std::is_same_v<T, type>, type&, type>;
  explicit async_completion(T& token)
      : handler_(
            static_cast<typename std::conditional_t<std::is_same_v<T, type>,
                                                    type&, T&&>>(token)),
        result_(handler_) {}
#else
  using handler_type = type;
  explicit async_completion(typename std::decay_t<T>& token)
      : handler_(token), result_(handler_) {}
  explicit async_completion(const typename std::decay_t<T>& token)
      : handler_(token), result_(handler_) {}
#endif

  handler_type handler_;
  async_result<typename std::decay_t<T>, S> result_;
};

namespace detail {

template <typename T, typename S>
struct async_result_helper
    : public async_result<typename std::decay<T>::type, S> {};

}  // namespace detail
}  // namespace boost::asio
#endif