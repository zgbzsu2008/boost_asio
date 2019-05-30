#ifndef BOOST_ASIO_IP_TCP_HPP
#define BOOST_ASIO_IP_TCP_HPP

#include <sys/socket.h>
#include <arpa/inet.h>

#include "basic_endpoint.hpp"

namespace boost::asio::ip
{
class tcp
{
 public:
  using endpoint = basic_endpoint<tcp>;

  static tcp v4() { return tcp(AF_INET); }

  int type() const { return SOCK_STREAM; }

  int protocol() const { return IPPROTO_TCP; }

  int family() const { return family_; }

 private:
  explicit tcp(int protocol_family) : family_(protocol_family) {}
  int family_;
};
}  // namespace boost::asio::ip
#endif