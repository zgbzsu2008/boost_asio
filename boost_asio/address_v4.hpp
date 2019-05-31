#ifndef BOOST_ASIO_IP_ADDRESS_V4_HPP
#define BOOST_ASIO_IP_ADDRESS_V4_HPP

#include <array>
#include <string>
#include <string.h>
#include <system_error>

#include "socket_types.hpp"
#include "socket_ops.hpp"

namespace boost::asio::ip
{
class address_v4
{
 public:
  using bytes_type = std::array<unsigned char, 4>;

  address_v4() { addr_.s_addr = 0; }
  explicit address_v4(const bytes_type& bytes) { memcpy(&addr_.s_addr, bytes.data(), 4); }
  explicit address_v4(uint32_t addr) { addr_.s_addr = ::htonl(static_cast<uint32_t>(addr)); }

  bytes_type to_bytes() const
  {
    bytes_type bytes;
    memcpy(bytes.data(), &addr_.s_addr, 4);
  }

  uint32_t to_uint() const { return static_cast<uint32_t>(::htonl(addr_.s_addr)); }

  std::string to_string() const
  {
    using namespace boost::asio::detail;
    std::error_code ec;
    char addr_str[max_addr_v4_str_len];
    const char* addr = socket_ops::inet_ntop(AF_INET, &addr_, addr_str, max_addr_v4_str_len, 0, ec);
    if(addr == 0) { throw(ec); }
    return addr;
  }

  static address_v4 make_address_v4(const char* str, std::error_code& ec)
  {
    using namespace boost::asio::detail;
    address_v4::bytes_type bytes;
    if(socket_ops::inet_pton(AF_INET, str, &bytes, 0, ec) <= 0) { return address_v4(); }
    return address_v4(bytes);
  }

  static address_v4 make_address_v4(const char* str)
  {
    std::error_code ec;
    address_v4 addr = make_address_v4(str, ec);
    if(ec) { throw(ec); }
    return addr;
  }

 private:
  boost::asio::detail::in4_addr_type addr_;
};
}  // namespace boost::asio::ip
#endif