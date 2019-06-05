#ifndef BOOST_ASIO_DETAIL_SOCKET_TYPES_HPP
#define BOOST_ASIO_DETAIL_SOCKET_TYPES_HPP

#include <iostream>

#include <arpa/inet.h>
#include <fcntl.h>
#include <net/if.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <unistd.h>

namespace boost::asio::detail {
/*
struct sockaddr
{
  sa_family_t sa_family;  // 2
  char sa_data[14];  // 14
};

struct sockaddr_in
{
  sa_family_t sin_family; // 2
  in_port_t sin_port;  // 2
  struct in_addr sin_addr;  // 4
  unsigned char sin_zero[sizeof(struct sockaddr) - sizeof(sa_family_t) -
sizeof(in_port_t) - sizeof(struct in_addr)];  // 8
};

struct sockaddr_in6
{
  sa_family_t sin6_family; // 2
  in_port_t sin6_port; // 2
  uint32_t sin6_flowinfo; // 4
  struct in6_addr sin6_addr; // 4
  uint32_t sin6_scope_id; // 4
};
*/

using socket_type = int;
using signed_size_type = long int;

using in4_addr_type = in_addr;
using in6_addr_type = in6_addr;

using socket_addr_type = sockaddr;
using sockaddr_in4_type = sockaddr_in;
using sockaddr_in6_type = sockaddr_in6;

using buffer = iovec;
constexpr int max_iov_len = 16;

constexpr int max_addr_v4_str_len = 256;
constexpr int max_addr_v6_str_len = 256;
constexpr int invalid_socket = -1;
constexpr int socket_error_retval = -1;
constexpr int custom_socket_option_level = 0xA5100000;
constexpr int enable_connection_aborted_option = 1;
constexpr int always_fail_option = 2;

}  // namespace boost::asio::detail

#endif
