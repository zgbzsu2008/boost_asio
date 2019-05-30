#ifndef BOOST_ASIO_IP_ADDRESS_HPP
#define BOOST_ASIO_IP_ADDRESS_HPP

#include <assert.h>
#include <string>

#include "address_v4.hpp"

namespace boost::asio::ip
{
class address
{
 public:
  address() : type_(ipv4), ipv4_address_() {}
  address(const address_v4& ipv4_address) : type_(ipv4), ipv4_address_(ipv4_address) {}

  bool is_v4() const { return type_ == ipv4; }

  address_v4 to_v4() const
  {
    assert(type_ == ipv4);
    return ipv4_address_;
  }

  std::string to_string() const { ipv4_address_.to_string(); }

  static address from_string(const std::string& ip)
  {
    return address(address_v4::make_address_v4(ip));
  }

 private:
  enum { ipv4, ipv6 } type_;
  address_v4 ipv4_address_;
};
}  // namespace boost::asio::ip
#endif
