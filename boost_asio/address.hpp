#ifndef BOOST_ASIO_IP_ADDRESS_HPP
#define BOOST_ASIO_IP_ADDRESS_HPP

#include <assert.h>
#include <string>

#include "address_v4.hpp"
#include "address_v6.hpp"

namespace boost::asio::ip {
class address
{
 public:
  address() : type_(ipv4), ipv4_address_() {}
  address(const address_v4& ipv4_address) : type_(ipv4), ipv4_address_(ipv4_address) {}
  address(const address_v6& ipv6_address) : type_(ipv6), ipv6_address_(ipv6_address) {}

  bool is_v4() const { return type_ == ipv4; }
  bool is_v6() const { return type_ == ipv6; }

  address_v4 to_v4() const
  {
    using namespace boost::asio::detail;
    throw_error(type_ != ipv4, std::make_error_code(std::errc::bad_address));
    return ipv4_address_;
  }

  address_v6 to_v6() const
  {
    using namespace boost::asio::detail;
    throw_error(type_ != ipv6, std::make_error_code(std::errc::bad_address));
    return ipv6_address_;
  }

  std::string to_string() const
  {
    return is_v4() ? ipv4_address_.to_string() : ipv6_address_.to_string();
  }

  static address make_address(const char* ip, std::error_code& ec)
  {
    address_v4 ipv4_addres = address_v4::make_address_v4(ip, ec);
    if (!ec) {
      return address(ipv4_addres);
    }

    ec = std::error_code();
    address_v6 ipv6_addres = address_v6::make_address_v6(ip, ec);
    if (!ec) {
      return address(ipv6_addres);
    }

    return address();
  }

  static address make_address(const char* ip)
  {
    std::error_code ec;
    address addr = make_address(ip, ec);
    boost::asio::detail::throw_error(ec, ec);
    return addr;
  }

 private:
  enum
  {
    ipv4,
    ipv6
  } type_;
  address_v4 ipv4_address_;
  address_v6 ipv6_address_;
};
}  // namespace boost::asio::ip
#endif