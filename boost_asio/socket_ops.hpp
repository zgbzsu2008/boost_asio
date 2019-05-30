#ifndef BOOST_ASIO_DETAIL_SOCKET_OPS_HPP
#define BOOST_ASIO_DETAIL_SOCKET_OPS_HPP

#include <system_error>
#include <memory>
#include <sys/uio.h>
#include <sys/socket.h>
#include <errno.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

namespace boost::asio::detail::socket_ops
{
enum {
  user_set_non_blocking = 1,
  internal_non_blocking = 2,
  non_blocking = user_set_non_blocking | internal_non_blocking,
  enable_connection_aborted = 4,
  user_set_linger = 8,
  stream_oriented = 16,
  datagram_oriented = 32,
  possible_dup = 64
};

using state_type = unsigned char;

int socket(int af, int type, int protocol, std::error_code& ec);

int accept(int s, struct sockaddr* addr, size_t* addrlen, std::error_code& ec);

int bind(int s, const struct sockaddr* addr, size_t addrlen, std::error_code& ec);

int close(int s, state_type& state, bool destruction, std::error_code& ec);

int shutdown(int s, int what, std::error_code& ec);

int connect(int s, const struct sockaddr* addr, size_t addrlen, std::error_code& ec);

int listen(int s, int backlog, std::error_code& ec);

ssize_t recv(int s, struct iovec* bufs, size_t count, int flags, std::error_code& ec);

ssize_t send(int s, const struct iovec* bufs, size_t count, int flags, std::error_code& ec);

int setsockopt(int s, state_type& state, int level, int optname, const void* optval, size_t optlen,
               std::error_code& ec);

int getsockopt(int s, state_type state, int level, int optname, void* optval, size_t* optlen,
               std::error_code& ec);

int getpeername(int s, struct sockaddr* addr, size_t* addrlen, bool cached, std::error_code& ec);

int getsockname(int s, struct sockaddr* addr, size_t* addrlen, std::error_code& ec);

const char* inet_ntop(int af, const void* src, char* dest, size_t length, unsigned long scope_id,
                      std::error_code& ec);

int inet_pton(int af, const char* src, void* dest, unsigned long* scope_id, std::error_code& ec);

uint32_t network_to_host_long(uint32_t value) { return ::ntohl(value); }
uint32_t host_to_network_long(uint32_t value) { return ::htonl(value); }

uint16_t network_to_host_short(uint16_t value) { return ::ntohs(value); }
uint16_t host_to_network_short(uint16_t value) { return ::htons(value); }

}  // namespace boost::asio::detail::socket_ops
#endif
