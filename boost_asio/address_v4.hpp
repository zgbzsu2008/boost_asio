#ifndef BOOST_ASIO_IP_ADDRESS_V4_HPP
#define BOOST_ASIO_IP_ADDRESS_V4_HPP

#include <string.h>
#include <array>
#include <limits>
#include <string>

#include "socket_ops.hpp"
#include "throw_error.hpp"

namespace boost::asio::ip {
class address_v4
{
 public:
  using bytes_type = std::array<unsigned char, 4>;

  address_v4() { addr_.s_addr = 0; }
  explicit address_v4(const bytes_type& bytes)
  {
    if ((std::numeric_limits<unsigned char>::max)() > 0xFF) {
      for (std::size_t i = 0; i < bytes.size(); ++i) {
        if (bytes[i] > 0xFF) {
          std::out_of_range ec("address_v4 from bytes_type");
          boost::asio::detail::throw_error(true, ec);
        }
      }
    }
    memcpy(&addr_.s_addr, bytes.data(), 4);
  }
  explicit address_v4(uint_least32_t addr)
  {
    using namespace boost::asio::detail;
    addr_.s_addr = socket_ops::host_to_network_long(addr);
  }

  bytes_type to_bytes() const
  {
    bytes_type bytes;
    memcpy(bytes.data(), &addr_.s_addr, 4);
  }

  uint32_t to_uint() const
  {
    using namespace boost::asio::detail;
    return socket_ops::host_to_network_long(addr_.s_addr);
  }

  std::string to_string() const
  {
    using namespace boost::asio::detail;
    std::error_code ec;
    char addr_str[max_addr_v4_str_len];
    const char* addr = socket_ops::inet_ntop(AF_INET, &addr_, addr_str, max_addr_v4_str_len, 0, ec);
    boost::asio::detail::throw_error(addr, ec);
    return addr;
  }

  static address_v4 make_address_v4(const char* str, std::error_code& ec)
  {
    using namespace boost::asio::detail;
    address_v4::bytes_type bytes;
    if (socket_ops::inet_pton(AF_INET, str, &bytes, 0, ec) <= 0) {
      return address_v4();
    }
    return address_v4(bytes);
  }

  static address_v4 make_address_v4(const char* str)
  {
    std::error_code ec;
    address_v4 addr = make_address_v4(str, ec);
    boost::asio::detail::throw_error(ec, ec);
    return addr;
  }

 private:
  boost::asio::detail::in4_addr_type addr_;
};
}  // namespace boost::asio::ip
#endif