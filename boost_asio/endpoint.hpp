#ifndef BOOST_ASIO_IP_DETAIL_ENDPOINT_HPP
#define BOOST_ASIO_IP_DETAIL_ENDPOINT_HPP

#include "address.hpp"

namespace boost::asio::ip::detail
{
class endpoint
{
 public:
  endpoint()
  {
    data_.v4.sin_family = AF_INET;
    data_.v4.sin_port = 0;
    data_.v4.sin_addr.s_addr = INADDR_ANY;
  }

  endpoint(int family, unsigned short port)
  {
    data_.v4.sin_family = AF_INET;
    data_.v4.sin_port = ::htons(port);
    data_.v4.sin_addr.s_addr = INADDR_ANY;
  }

  endpoint(const address& addr, unsigned short port)
  {
    data_.v4.sin_family = AF_INET;
    data_.v4.sin_port = ::htons(port);
    data_.v4.sin_addr = addr.
  }

  bool is_v4() const { return data_.v4.sin_family == AF_INET; }

  unsigned short port() const { return ::ntohs(data_.v4.sin_port); }
  void port(unsigned short port) { data_.v4.sin_port = ::htons(port); }

  ip::address address() const { return ip::address(address_v4(data_.v4.sin_addr)); }
  void address(const ip::address& addr) { data_.v4.sin_addr = addr.to_v4().get_addr(); }

 private:
  union {
    boost::asio::detail::socket_addr_type base;
    boost::asio::detail::sockaddr_in4_type v4;
    boost::asio::detail::sockaddr_in6_type v6;
  } data_;
};
}  // namespace boost::asio::ip::detail
#endif