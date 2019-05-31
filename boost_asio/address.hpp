#ifndef BOOST_ASIO_IP_ADDRESS_HPP
#define BOOST_ASIO_IP_ADDRESS_HPP

#include <assert.h>
#include <string>

#include "address_v4.hpp"
#include "address_v6.hpp"

namespace boost::asio::ip
{
class address
{
 public:
  address() : type_(ipv4), ipv4_address_() {}
  address(const address_v4& ipv4_address) : type_(ipv4), ipv4_address_(ipv4_address) {}

  bool is_v4() const { return type_ == ipv4; }
  bool is_v6() const { return type_ == ipv6; }

  address_v4 to_v4() const
  {
    if(type_ != ipv4) { throw std::make_error_code(std::errc::bad_address); }
    return ipv4_address_;
  }

  address_v6 to_v6() const
  {
    if(type_ != ipv6) { throw std::make_error_code(std::errc::bad_address); }
    return ipv6_address_;
  }

  std::string to_string() const
  {
    if(is_v4()) { ipv4_address_.to_string(); }
    // if(is_v6()) { ipv6_address_.to_string(); }
    return std::string();
  }

  static address from_string(const std::string& ip)
  {
    std::error_code ec;
    address_v4 ipv4 = address_v4::make_address_v4(ip.c_str(), ec);
    if(!ec) { return address(ipv4); }

    return address();
  }

 private:
  enum { ipv4, ipv6 } type_;
  address_v4 ipv4_address_;
  address_v6 ipv6_address_;
};
}  // namespace boost::asio::ip
#endif