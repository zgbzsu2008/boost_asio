#ifndef BOOST_ASIO_DETAIL_SOCKET_OPS_HPP
#define BOOST_ASIO_DETAIL_SOCKET_OPS_HPP

#include <system_error>
#include <errno.h>
#include <string.h>
#include <memory>

#include "socket_types.hpp"

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

struct noop_deleter
{
  void operator()(void*) {}
};

using shared_cancel_token_type = std::shared_ptr<void>;
using weak_cancel_token_type = std::weak_ptr<void>;

int socket(int af, int type, int protocol, std::error_code& ec);

int bind(socket_type sockfd, const socket_addr_type* addr, std::size_t addrlen,
         std::error_code& ec);

int listen(socket_type sockfd, int backlog, std::error_code& ec);

int accept(socket_type sockfd, socket_addr_type* addr, std::size_t* addrlen, std::error_code& ec);

int shutdown(socket_type sockfd, int what, std::error_code& ec);

int close(socket_type sockfd, state_type& state, bool destruction, std::error_code& ec);

int connect(socket_type sockfd, const socket_addr_type* addr, std::size_t addrlen,
            std::error_code& ec);

ssize_t recv(socket_type sockfd, buffer* bufs, std::size_t count, int flags, std::error_code& ec);

ssize_t send(socket_type sockfd, const buffer* bufs, std::size_t count, int flags,
             std::error_code& ec);

int setsockopt(socket_type sockfd, state_type& state, int level, int optname, const void* optval,
               std::size_t optlen, std::error_code& ec);

int getsockopt(socket_type sockfd, state_type state, int level, int optname, void* optval,
               std::size_t* optlen, std::error_code& ec);

int getpeername(socket_type sockfd, socket_addr_type* addr, std::size_t* addrlen, bool /*cached*/,
                std::error_code& ec);

int getsockname(socket_type sockfd, socket_addr_type* addr, std::size_t* addrlen,
                std::error_code& ec);

const char* inet_ntop(int af, const void* src, char* dest, std::size_t length,
                      unsigned long /*scope_id*/, std::error_code& ec);

int inet_pton(int af, const char* src, void* dest, unsigned long* /*scope_id*/,
              std::error_code& ec);

}  // namespace boost::asio::detail::socket_ops
#endif