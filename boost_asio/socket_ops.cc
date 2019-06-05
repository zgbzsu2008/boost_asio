#include "socket_ops.hpp"

namespace boost::asio::detail::socket_ops {

inline void clear_last_error() { errno = 0; }

inline void clear_current_error(bool flag, std::error_code& ec)
{
  if (flag) {
    ec = std::error_code();
  }
}

template <typename ReturnType>
inline ReturnType error_wrapper(ReturnType value, std::error_code& ec)
{
  ec = std::error_code(errno, std::generic_category());
  return value;
}

template <typename SockLenType>
inline int call_accept(SockLenType msghdr::*, socket_type sockfd, socket_addr_type* addr,
                       std::size_t* addrlen)
{
  SockLenType len = addrlen ? static_cast<SockLenType>(*addrlen) : 0;
  int result = ::accept(sockfd, addr, addrlen ? &len : 0);
  if (addrlen) {
    *addrlen = static_cast<std::size_t>(len);
  }
  return result;
}

template <typename SockLenType>
inline int call_bind(SockLenType msghdr::*, socket_type sockfd, const socket_addr_type* addr,
                     std::size_t addrlen)
{
  return ::bind(sockfd, addr, static_cast<SockLenType>(addrlen));
}

template <typename SockLenType>
inline int call_connect(SockLenType msghdr::*, socket_type sockfd, const socket_addr_type* addr,
                        std::size_t addrlen)
{
  return ::connect(sockfd, addr, static_cast<SockLenType>(addrlen));
}

template <typename SockLenType>
inline int call_setsockopt(SockLenType msghdr::*, socket_type sockfd, int level, int optname,
                           const void* optval, std::size_t optlen)
{
  SockLenType len = static_cast<SockLenType>(optlen);
  return ::setsockopt(sockfd, level, optname, optval, len);
}

template <typename SockLenType>
inline int call_getsockopt(SockLenType msghdr::*, socket_type sockfd, int level, int optname,
                           void* optval, std::size_t* optlen)
{
  SockLenType len = static_cast<SockLenType>(*optlen);
  int result = ::getsockopt(sockfd, level, optname, optval, &len);
  *optlen = static_cast<std::size_t>(len);
  return result;
}

template <typename SockLenType>
inline int call_getpeername(SockLenType msghdr::*, socket_type sockfd, socket_addr_type* addr,
                            std::size_t* addrlen)
{
  SockLenType len = static_cast<SockLenType>(*addrlen);
  int result = ::getpeername(sockfd, addr, &len);
  *addrlen = static_cast<std::size_t>(len);
  return result;
}

template <typename SockLenType>
inline int call_getsockname(SockLenType msghdr::*, int s, socket_addr_type* addr,
                            std::size_t* addrlen)
{
  SockLenType len = static_cast<SockLenType>(*addrlen);
  int result = ::getsockname(s, addr, &len);
  *addrlen = static_cast<std::size_t>(len);
  return result;
}

int socket(int af, int type, int protocol, std::error_code& ec)
{
  clear_last_error();
  socket_type sockfd = error_wrapper(::socket(af, type, protocol), ec);
  clear_current_error(sockfd != invalid_socket, ec);
  return sockfd;
}

int accept(socket_type sockfd, socket_addr_type* addr, std::size_t* addrlen, std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  clear_last_error();
  int new_sockfd = error_wrapper(call_accept(&msghdr::msg_namelen, sockfd, addr, addrlen), ec);
  clear_current_error(new_sockfd != invalid_socket, ec);
  return new_sockfd;
}

int bind(socket_type sockfd, const socket_addr_type* addr, std::size_t addrlen, std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  clear_last_error();
  int result = error_wrapper(call_bind(&msghdr::msg_namelen, sockfd, addr, addrlen), ec);
  clear_current_error(result == 0, ec);
  return result;
}

int close(socket_type sockfd, state_type& state, bool destruction, std::error_code& ec)
{
  int result = 0;
  if (sockfd != invalid_socket) {
    if (destruction && (state & user_set_linger)) {
      //
    }

    clear_last_error();
    result = error_wrapper(::close(sockfd), ec);
    if (result != 0 && ((ec != std::errc::operation_would_block) ||
                        (ec != std::errc::resource_unavailable_try_again))) {
      int flags = ::fcntl(sockfd, F_GETFL, 0);
      if (flags >= 0) {
        ::fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK);
      }
      state &= ~non_blocking;

      clear_last_error();
      result = error_wrapper(::close(sockfd), ec);
    }
  }
  clear_current_error(result == 0, ec);
  return result;
}

int shutdown(socket_type sockfd, int what, std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  clear_last_error();
  int result = error_wrapper(::shutdown(sockfd, what), ec);
  clear_current_error(result == 0, ec);
  return result;
}

int connect(socket_type sockfd, const socket_addr_type* addr, std::size_t addrlen,
            std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  clear_last_error();
  int result = error_wrapper(call_connect(&msghdr::msg_namelen, sockfd, addr, addrlen), ec);
  clear_current_error(result == 0, ec);
  if (result > 0 && ec == std::errc::resource_unavailable_try_again) {
    ec = std::make_error_code(std::errc::not_enough_memory);
  }
  return result;
}

int listen(socket_type sockfd, int backlog, std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  clear_last_error();
  int result = error_wrapper(::listen(sockfd, backlog), ec);
  clear_current_error(result == 0, ec);
  return result;
}

std::size_t recv(socket_type sockfd, buffer* bufs, std::size_t count, int flags,
                 std::error_code& ec)
{
  clear_last_error();
  msghdr msg = msghdr();
  msg.msg_iov = bufs;
  msg.msg_iovlen = static_cast<int>(count);
  std::size_t result = error_wrapper(::recvmsg(sockfd, &msg, flags), ec);
  clear_current_error(result == 0, ec);
  return result;
}

std::size_t sync_recv(socket_type sockfd, state_type state, buffer* bufs, std::size_t count,
                      int flags, bool all_empty, std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return 0;
  }

  if (all_empty && (state & stream_oriented)) {
    ec = std::error_code();
    return 0;
  }

  for (;;) {
    signed_size_type bytes = socket_ops::recv(sockfd, bufs, count, flags, ec);
    if (bytes > 0) {
      return bytes;
    }

    if ((state & stream_oriented) && bytes == 0) {
      ec = std::make_error_code(std::errc::invalid_seek);
      return 0;
    }

    if ((state & stream_oriented) && ((ec != std::errc::operation_would_block) ||
                                      (ec != std::errc::resource_unavailable_try_again))) {
      return 0;
    }

    if (socket_ops::poll_read(sockfd, 0, -1, ec) < 0) {
      return 0;
    }
  }
}

std::size_t send(socket_type sockfd, const buffer* bufs, std::size_t count, int flags,
                 std::error_code& ec)
{
  msghdr msg = msghdr();
  msg.msg_iov = const_cast<buffer*>(bufs);
  msg.msg_iovlen = static_cast<int>(count);
  flags |= MSG_NOSIGNAL;

  clear_last_error();
  std::size_t result = error_wrapper(::sendmsg(sockfd, &msg, flags), ec);
  clear_current_error(result == 0, ec);
  return result;
}

std::size_t sync_send(socket_type sockfd, state_type state, const buffer* bufs, size_t count,
                      int flags, bool all_empty, std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return 0;
  }

  for (;;) {
    signed_size_type bytes = socket_ops::send(sockfd, bufs, count, flags, ec);
    if (bytes >= 0) {
      return bytes;
    }

    if ((state & user_set_non_blocking) || ((ec != std::errc::operation_would_block) &&
                                            (ec != std::errc::resource_unavailable_try_again))) {
      return 0;
    }

    if (socket_ops::poll_write(sockfd, 0, -1, ec) < 0) {
      return 0;
    }
  }
}

int setsockopt(socket_type sockfd, state_type& state, int level, int optname, const void* optval,
               std::size_t optlen, std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  if (level == custom_socket_option_level && optname == always_fail_option) {
    ec = std::make_error_code(std::errc::invalid_argument);
    return socket_error_retval;
  }

  if (level == custom_socket_option_level && optname == enable_connection_aborted_option) {
    if (optlen != sizeof(int)) {
      ec = std::make_error_code(std::errc::invalid_argument);
      return socket_error_retval;
    }

    if (*static_cast<const int*>(optval)) {
      state |= enable_connection_aborted;
    } else {
      state |= ~enable_connection_aborted;
    }
    ec = std::error_code();
    return 0;
  }

  if (level == SOL_SOCKET && optname == SO_LINGER) {
    state |= user_set_linger;
  }

  clear_last_error();
  int result = error_wrapper(
      call_setsockopt(&msghdr::msg_namelen, sockfd, level, optname, optval, optlen), ec);
  if (result == 0) {
    ec = std::error_code();
  }
  return result;
}

int getsockopt(socket_type sockfd, state_type state, int level, int optname, void* optval,
               std::size_t* optlen, std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  if (level == custom_socket_option_level && optname == always_fail_option) {
    ec = std::make_error_code(std::errc::invalid_argument);
    return socket_error_retval;
  }

  if (level == custom_socket_option_level && optname == enable_connection_aborted_option) {
    if (*optlen != sizeof(int)) {
      ec = std::make_error_code(std::errc::invalid_argument);
      return socket_error_retval;
    }

    *static_cast<int*>(optval) = (state & enable_connection_aborted) ? 1 : 0;
    ec = std::error_code();
    return 0;
  }

  clear_last_error();
  int result = error_wrapper(
      call_getsockopt(&msghdr::msg_namelen, sockfd, level, optname, optval, optlen), ec);

  if (result == 0 && level == SOL_SOCKET && *optlen == sizeof(int) &&
      (optname == SO_SNDBUF || optname == SO_RCVBUF)) {
    *static_cast<int*>(optval) /= 2;
  }

  clear_current_error(result == 0, ec);
  return result;
}

int getpeername(socket_type sockfd, socket_addr_type* addr, std::size_t* addrlen, bool /*cached*/,
                std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  clear_last_error();
  int result = error_wrapper(call_getpeername(&msghdr::msg_namelen, sockfd, addr, addrlen), ec);
  clear_current_error(result == 0, ec);
  return result;
}

int getsockname(socket_type sockfd, socket_addr_type* addr, std::size_t* addrlen,
                std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  clear_last_error();
  int result = error_wrapper(call_getsockname(&msghdr::msg_namelen, sockfd, addr, addrlen), ec);
  clear_current_error(result == 0, ec);
  return result;
}

const char* inet_ntop(int af, const void* src, char* dest, size_t length, unsigned long scope_id,
                      std::error_code& ec)
{
  clear_last_error();
  const char* result = error_wrapper(::inet_ntop(af, src, dest, static_cast<int>(length)), ec);
  if (result == 0 && !ec) {
    ec = std::make_error_code(std::errc::invalid_argument);
  }

  if (result != 0 && af == AF_INET6 && scope_id != 0) {
    char if_name[(IF_NAMESIZE > 21 ? IF_NAMESIZE : 21) + 1] = "%";
    const in6_addr_type* ipv6_address = static_cast<const in6_addr_type*>(src);
    bool is_link_local =
        ((ipv6_address->s6_addr[0] == 0xfe) && ((ipv6_address->s6_addr[1] & 0xc0) == 0x80));
    bool is_multicast_link_local =
        ((ipv6_address->s6_addr[0] == 0xff) && ((ipv6_address->s6_addr[1] & 0x0f) == 0x02));

    if ((!is_link_local && !is_multicast_link_local) ||
        if_indextoname(static_cast<unsigned>(scope_id), if_name + 1) == 0) {
      sprintf(if_name + 1, "%lu", scope_id);
    }
    strcat(dest, if_name);
  }
  return result;
}

int inet_pton(int af, const char* src, void* dest, unsigned long* scope_id, std::error_code& ec)
{
  clear_last_error();
  const bool is_v6 = (af == AF_INET6);
  const char* if_name = is_v6 ? strchr(src, '%') : 0;
  char src_buf[max_addr_v6_str_len + 1];
  const char* src_ptr = src;
  if (if_name != 0) {
    if (if_name - src > max_addr_v6_str_len) {
      ec = std::make_error_code(std::errc::invalid_argument);
      return 0;
    }
    memcpy(src_buf, src, if_name - src);
    src_buf[if_name - src] = 0;
    src_ptr = src_buf;
  }

  int result = error_wrapper(::inet_pton(af, src_ptr, dest), ec);
  if (result <= 0 && !ec) {
    ec = std::make_error_code(std::errc::invalid_argument);
  }

  if (result > 0 && is_v6 && scope_id) {
    *scope_id = 0;
    if (if_name != 0) {
      in6_addr_type* ipv6_address = static_cast<in6_addr_type*>(dest);
      bool is_link_local =
          ((ipv6_address->s6_addr[0] == 0xfe) && ((ipv6_address->s6_addr[1] & 0xc0) == 0x80));
      bool is_multicast_link_local =
          ((ipv6_address->s6_addr[0] == 0xff) && ((ipv6_address->s6_addr[1] & 0x0f) == 0x02));

      if (is_link_local || is_multicast_link_local) {
        *scope_id = if_nametoindex(if_name + 1);
      }

      if (*scope_id == 0) {
        *scope_id = atoi(if_name + 1);
      }
    }
  }

  return result;
}

uint32_t network_to_host_long(uint32_t value)
{
#if defined(BOOST_ASIO_WINDOWS_RUNTIME)
  unsigned char* value_p = reinterpret_cast<unsigned char*>(&value);
  uint32_t result =
      (static_cast<u_long_type>(value_p[0]) << 24) | (static_cast<u_long_type>(value_p[1]) << 16) |
      (static_cast<u_long_type>(value_p[2]) << 8) | static_cast<u_long_type>(value_p[3]);
  return result;
#else
  return ::ntohl(value);
#endif
}

uint32_t host_to_network_long(uint32_t value)
{
#if defined(BOOST_ASIO_WINDOWS_RUNTIME)
  uint32_t result;
  unsigned char* result_p = reinterpret_cast<unsigned char*>(&result);
  result_p[0] = static_cast<unsigned char>((value >> 24) & 0xFF);
  result_p[1] = static_cast<unsigned char>((value >> 16) & 0xFF);
  result_p[2] = static_cast<unsigned char>((value >> 8) & 0xFF);
  result_p[3] = static_cast<unsigned char>(value & 0xFF);
  return result;
#else
  return ::htonl(value);
#endif
}

uint16_t network_to_host_short(uint16_t value)
{
#if defined(BOOST_ASIO_WINDOWS_RUNTIME)
  unsigned char* value_p = reinterpret_cast<unsigned char*>(&value);
  uint16_t result =
      (static_cast<u_short_type>(value_p[0]) << 8) | static_cast<u_short_type>(value_p[1]);
  return result;
#else
  return ::ntohs(value);
#endif
}

uint16_t host_to_network_short(uint16_t value)
{
#if defined(BOOST_ASIO_WINDOWS_RUNTIME)
  uint16_t result;
  unsigned char* result_p = reinterpret_cast<unsigned char*>(&result);
  result_p[0] = static_cast<unsigned char>((value >> 8) & 0xFF);
  result_p[1] = static_cast<unsigned char>(value & 0xFF);
  return result;
#else
  return ::htons(value);
#endif
}

int poll_read(socket_type sockfd, state_type state, int msec, std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  pollfd fds{sockfd, POLLIN, 0};
  int timeout = (state & user_set_non_blocking) ? 0 : -1;

  clear_last_error();
  int result = error_wrapper(::poll(&fds, 1, timeout), ec);
  clear_current_error(true, ec);
  if (result == 0) {
    if (state & user_set_non_blocking) {
      ec = std::make_error_code(std::errc::operation_would_block);
    }
  }
  return result;
}

int poll_write(socket_type sockfd, state_type state, int msec, std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  pollfd fds{sockfd, POLLOUT, 0};
  int timeout = (state & user_set_non_blocking) ? 0 : -1;

  clear_last_error();
  int result = error_wrapper(::poll(&fds, 1, timeout), ec);
  clear_current_error(true, ec);
  if (result == 0) {
    if (state & user_set_non_blocking) {
      ec = std::make_error_code(std::errc::operation_would_block);
    }
  }
  return result;
}

int poll_error(int sockfd, state_type state, std::error_code& ec)
{
  if (sockfd == invalid_socket) {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  pollfd fds{sockfd, POLLPRI | POLLERR | POLLHUP, 0};
  int timeout = (state & user_set_non_blocking) ? 0 : -1;

  clear_last_error();
  int result = error_wrapper(::poll(&fds, 1, timeout), ec);
  clear_current_error(true, ec);
  if (result == 0) {
    if (state & user_set_non_blocking) {
      ec = std::make_error_code(std::errc::operation_would_block);
    }
  }
  return result;
}

bool non_blocking_send(socket_type sockfd, const buffer* bufs, size_t count, int flags,
                       std::error_code& ec, size_t& bytes_transferred)
{
  for (;;) {
    signed_size_type bytes = socket_ops::send(sockfd, bufs, count, flags, ec);
    if (ec == std::errc::interrupted) {
      continue;
    }

    if ((ec == std::errc::operation_would_block) ||
        ec == std::errc::resource_unavailable_try_again) {
      return false;
    }

    clear_current_error(bytes >= 0, ec);
    bytes_transferred = bytes >= 0 ? bytes : 0;

    return true;
  }
}
}  // namespace boost::asio::detail::socket_ops