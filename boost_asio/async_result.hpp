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

  using handler_type = typename std::conditional<std::is_same<T, type>::value,
                                                 type&, type>::type;

 public:
  explicit async_completion(T& token)
      : handler_(
            static_cast<typename std::conditional<std::is_same<T, type>::value,
                                                  type&, T&&>::type>(token)),
        result_(handler_) {}

  handler_type handler_;
  async_result<typename std::decay<T>::type, S> result_;
};

namespace detail {

template <typename T, typename S>
struct async_result_helper
    : public async_result<typename std::decay<T>::type, S> {};

}  // namespace detail
}  // namespace boost::asio
#endif