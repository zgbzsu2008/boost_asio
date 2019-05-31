#ifndef BOOST_ASIO_DETAIL_SOCKET_TYPES_HPP
#define BOOST_ASIO_DETAIL_SOCKET_TYPES_HPP

#include <unistd.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

namespace boost::asio::detail
{
using socket_type = int;
using socket_addr_type = sockaddr;
using sockaddr_in4_type = sockaddr_in;
using sockaddr_in6_type = sockaddr_in6;
using in4_addr_type = in_addr;
using in6_addr_type = in6_addr;
using buffer = iovec;

constexpr int max_addr_v4_str_len = 256;
constexpr int max_addr_v6_str_len = 256;
constexpr int invalid_socket = -1;
constexpr int socket_error_retval = -1;
constexpr int custom_socket_option_level = 0xA5100000;
constexpr int enable_connection_aborted_option = 1;
constexpr int always_fail_option = 2;
}  // namespace boost::asio::detail

#endif
