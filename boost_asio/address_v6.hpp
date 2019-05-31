#ifndef BOOST_ASIO_IP_ADDRESS_V6_HPP
#define BOOST_ASIO_IP_ADDRESS_V6_HPP

#include <array>
#include <string.h>
#include <system_error>

#include "socket_types.hpp"
#include "socket_ops.hpp"

namespace boost::asio::ip
{
class address_v6
{
 public:
  using bytes_type = std::array<unsigned char, 16>;

  address_v6() : addr_(), scope_id_(0) {}
  explicit address_v6(const bytes_type& bytes, unsigned long scope_id = 0) : scope_id_(scope_id)
  {
    memcpy(&addr_.s6_addr, bytes.data(), 16);
  }

  bytes_type to_bytes() const
  {
    bytes_type bytes;
    memcpy(bytes.data(), &addr_.s6_addr, 16);
  }

  std::string to_string() const;
  static address_v6 make_address_v4(const char* str, std::error_code& ec);
  static address_v6 make_address_v4(const char* str);

 private:
  boost::asio::detail::in6_addr_type addr_;
  unsigned long scope_id_;
};
}  // namespace boost::asio::ip
#endif
