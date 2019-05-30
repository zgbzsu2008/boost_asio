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
    v4.sin_family = AF_INET;
    v4.sin_port = 0;
    v4.sin_addr.s_addr = INADDR_ANY;
  }

  endpoint(int family, unsigned short port)
  {
    v4.sin_family = AF_INET;
    v4.sin_port = ::htons(port);
    v4.sin_addr.s_addr = INADDR_ANY;
  }

  endpoint(const address& addr, unsigned short port)
  {
    v4.sin_family = AF_INET;
    v4.sin_port = ::htons(port);
    v4.sin_addr = addr.to_v4().get_addr();
  }

  bool is_v4() const { return v4.sin_family == AF_INET; }

  unsigned short port() const { return ::ntohs(v4.sin_port); }
  void port(unsigned short port) { v4.sin_port = ::htons(port); }

  ip::address address() const { return ip::address(address_v4(v4.sin_addr)); }
  void address(const ip::address& addr) { v4.sin_addr = addr.to_v4().get_addr(); }

 private:
  union {
    struct sockaddr_in v4;
    // struct sockaddr_in6 v6;
  };
};
}  // namespace boost::asio::ip::detail
#endif