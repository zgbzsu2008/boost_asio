#ifndef BOOST_ASIO_DETAIL_THROW_ERROR_HPP
#define BOOST_ASIO_DETAIL_THROW_ERROR_HPP

#include <stdexcept>
#include <system_error>

namespace boost::asio::detail {
template <typename Flag, typename Error>
inline void throw_error(const Flag& flag, const Error& ec)
{
  if (flag) {
    throw(ec);
  }
}

}  // namespace boost::asio::detail
#endif