#ifndef BOOST_ASIO_IP_BASIC_ENDPOINT_HPP
#define BOOST_ASIO_IP_BASIC_ENDPOINT_HPP

#include "address.hpp"
#include "endpoint.hpp"
#include "tcp.hpp"

namespace boost::asio::ip {
template <typename T>
class basic_endpoint
{
 public:
  using protocol_type = T;

  basic_endpoint() : impl_() {}
  basic_endpoint(const protocol_type& protocol, unsigned short port) : impl_(protocol.family, port)
  {}
  basic_endpoint(const address& addr, unsigned short port) : impl_(addr, port) {}

  protocol_type protocol() const { return protocol_type::v4(); }

  unsigned short port() const { return impl_.port(); }
  void port(unsigned short port) { impl_.port(port); }

  ip::address address() const { return impl_.address(); }
  void address(const ip::address& addr) { impl_.address(addr); }

 private:
  detail::endpoint impl_;
};
}  // namespace boost::asio::ip
#endif
