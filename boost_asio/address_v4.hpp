#ifndef BOOST_ASIO_IP_ADDRESS_V4_HPP
#define BOOST_ASIO_IP_ADDRESS_V4_HPP

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string>

namespace boost::asio::ip
{
class address_v4
{
 public:
  address_v4() { addr_.s_addr = 0; }
  explicit address_v4(in_addr addr) { addr_ = addr; }

  struct in_addr get_addr() const { return addr_; }
  void set_addr(struct in_addr addr) { addr_ = addr; }

  std::string to_string() const { return std::string(::inet_ntoa(addr_)); }

  static address_v4 make_address_v4(const std::string& addr)
  {
    struct in_addr in;
    ::inet_pton(AF_INET, addr.c_str(), &in.s_addr);
    return address_v4(in);
  }

 private:
  struct in_addr addr_;
};
}  // namespace boost::asio::ip
#endif