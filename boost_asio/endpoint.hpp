#ifndef BOOST_ASIO_IP_DETAIL_ENDPOINT_HPP
#define BOOST_ASIO_IP_DETAIL_ENDPOINT_HPP

#include <iostream>

#include "address.hpp"

namespace boost::asio::ip::detail {
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
    using namespace boost::asio::detail;
    if (family == AF_INET) {
      data_.v4.sin_family = AF_INET;
      data_.v4.sin_port = socket_ops::host_to_network_short(port);
      data_.v4.sin_addr.s_addr = INADDR_ANY;
    } else {
      data_.v6.sin6_family = AF_INET6;
      data_.v6.sin6_port = socket_ops::host_to_network_short(port);
      data_.v6.sin6_flowinfo = 0;
      memset(&data_.v6.sin6_addr, 0, 16);
      data_.v6.sin6_scope_id = 0;
    }
  }

  endpoint(const address& addr, unsigned short port)
  {
    using namespace boost::asio::detail;
    if (addr.is_v4()) {
      data_.v4.sin_family = AF_INET;
      data_.v4.sin_port = socket_ops::host_to_network_short(port);
      address_v4 ipv4_addr = addr.to_v4();
      data_.v4.sin_addr.s_addr = socket_ops::host_to_network_long(ipv4_addr.to_uint());
      data_.v4.sin_addr.s_addr = INADDR_ANY;
    } else {
      data_.v6.sin6_family = AF_INET6;
      data_.v6.sin6_port = socket_ops::host_to_network_short(port);
      data_.v6.sin6_flowinfo = 0;
      address_v6 ipv6_addr = addr.to_v6();
      address_v6::bytes_type bytes = ipv6_addr.to_bytes();
      memcpy(data_.v6.sin6_addr.s6_addr, bytes.data(), 16);
      data_.v6.sin6_scope_id = static_cast<uint32_t>(ipv6_addr.scope_id());
    }
  }

  bool is_v4() const { return data_.base.sa_family == AF_INET; }

  unsigned short port() const
  {
    using namespace boost::asio::detail;
    uint16_t val = is_v4() ? data_.v4.sin_port : data_.v6.sin6_port;
    return socket_ops::network_to_host_short(val);
  }

  void port(unsigned short port)
  {
    using namespace boost::asio::detail;
    uint16_t val = socket_ops::host_to_network_short(port);
    if (is_v4()) {
      data_.v4.sin_port = val;
    } else {
      data_.v6.sin6_port = val;
    }
  }

  ip::address address() const
  {
    using namespace boost::asio::detail;
    if (is_v4()) {
      return address_v4(socket_ops::network_to_host_long(data_.v4.sin_addr.s_addr));
    } else {
      address_v6::bytes_type bytes;
      memcpy(bytes.data(), data_.v6.sin6_addr.s6_addr, 16);
      return address_v6(bytes, data_.v6.sin6_scope_id);
    }
  }

  void address(const ip::address& addr)
  {
    endpoint tmp(addr, port());
    data_ = tmp.data_;
  }

  std::string to_string() const
  {
    std::cout << '[' << address().to_string() << "]:" << port() << std::endl;
  }

 private:
  union
  {
    boost::asio::detail::socket_addr_type base;
    boost::asio::detail::sockaddr_in4_type v4;
    boost::asio::detail::sockaddr_in6_type v6;
  } data_;
};
}  // namespace boost::asio::ip::detail
#endif