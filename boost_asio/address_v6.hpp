#ifndef BOOST_ASIO_IP_ADDRESS_V6_HPP
#define BOOST_ASIO_IP_ADDRESS_V6_HPP

#include <string.h>
#include <array>
#include <limits>
#include <string>

#include "socket_ops.hpp"
#include "throw_error.hpp"

namespace boost::asio::ip {
class address_v6
{
 public:
  using bytes_type = std::array<unsigned char, 16>;

  address_v6() : addr_(), scope_id_(0) {}
  explicit address_v6(const bytes_type& bytes, unsigned long scope_id = 0) : scope_id_(scope_id)
  {
    if ((std::numeric_limits<unsigned char>::max)() > 0xFF) {
      for (std::size_t i = 0; i < bytes.size(); ++i) {
        if (bytes[i] > 0xFF) {
          std::out_of_range ec("address_v6 from bytes_type");
          boost::asio::detail::throw_error(true, ec);
        }
      }
    }
    memcpy(&addr_.s6_addr, bytes.data(), 16);
  }
  unsigned long scope_id() const { return scope_id_; }
  void scope_id(unsigned long id) { scope_id_ = id; }

  bytes_type to_bytes() const
  {
    bytes_type bytes;
    memcpy(bytes.data(), &addr_.s6_addr, 16);
  }

  std::string to_string() const
  {
    using namespace boost::asio::detail;
    std::error_code ec;
    char addr_str[max_addr_v6_str_len];
    const char* addr =
        socket_ops::inet_ntop(AF_INET6, &addr_, addr_str, max_addr_v6_str_len, scope_id_, ec);
    throw_error(addr, ec);
    return addr;
  }

  static address_v6 make_address_v6(const char* ip, std::error_code& ec)
  {
    using namespace boost::asio::detail;
    address_v6::bytes_type bytes;
    unsigned long scope_id = 0;
    if (socket_ops::inet_pton(AF_INET6, ip, bytes.data(), &scope_id, ec) <= 0) {
      return address_v6();
    }
    return address_v6();
  }

  static address_v6 make_address_v6(const char* ip)
  {
    std::error_code ec;
    address_v6 addr = make_address_v6(ip, ec);
    boost::asio::detail::throw_error(ec, ec);
    return addr;
  }

 private:
  boost::asio::detail::in6_addr_type addr_;
  unsigned long scope_id_;
};
}  // namespace boost::asio::ip
#endif
