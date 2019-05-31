#include "socket_ops.hpp"

namespace boost::asio::detail::socket_ops
{
namespace detail
{
inline void clear_last_error() { errno = 0; }

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
  if(addrlen) { *addrlen = static_cast<std::size_t>(len); }
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
  return ::connect(s, addr, static_cast<SockLenType>(addrlen));
}

template <typename SockLenType>
inline int call_setsockopt(SockLenType msghdr::*, socket_type sockfd, int level, int optname,
                           const void* optval, std::size_t optlen)
{
  SockLenType len = static_cast<SockLenType>(*optlen);
  return ::setsockopt(s, level, optname, static_cast<char*>(optval), len);
}

template <typename SockLenType>
inline int call_getsockopt(SockLenType msghdr::*, socket_type sockfd, int level, int optname,
                           void* optval, std::size_t* optlen)
{
  SockLenType len = static_cast<SockLenType>(*optlen);
  int result = ::getsockopt(s, level, optname, static_cast<char*>(optval), &len);
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
}  // namespace detail

int socket(int af, int type, int protocol, std::error_code& ec)
{
  detail::clear_last_error();
  socket_type sockfd = detail::error_wrapper(::socket(af, type, protocol), ec);
  if(sockfd != invalid_socket) { ec = std::error_code(); }
  return sockfd;
}

int accept(socket_type sockfd, socket_addr_type* addr, std::size_t* addrlen, std::error_code& ec)
{
  if(sockfd == invalid_socket)
  {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }
  detail::clear_last_error();

  int new_sockfd =
    detail::error_wrapper(detail::call_accept(&msghdr::msg_namelen, sockfd, addr, addrlen), ec);
  if(new_sockfd == invalid_socket) { return socket_error_retval; }
  ec = std::error_code();
  return new_sockfd;
}

int bind(socket_type sockfd, const socket_addr_type* addr, std::size_t addrlen, std::error_code& ec)
{
  if(sockfd == invalid_socket)
  {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }
  detail::clear_last_error();

  int result =
    detail::error_wrapper(detail::call_bind(&msghdr::msg_namelen, sockfd, addr, addrlen), ec);
  if(result == 0) { ec = std::error_code(); }
  return result;
}

int close(socket_type sockfd, state_type& state, bool destruction, std::error_code& ec)
{
  int result = 0;
  if(sockfd != invalid_socket)
  {
    if(destruction && (state & user_set_linger))
    {
      //
    }

    detail::clear_last_error();
    result = detail::error_wrapper(::close(sockfd), ec);
    if(result != 0 && ((ec == std::errc::operation_would_block) || (ec == std::errc::interrupted)))
    {
      int flags = ::fcntl(sockfd, F_GETFL, 0);
      if(flags >= 0) { ::fcntl(sockfd, F_SETFL, flags & ~O_NONBLOCK); }
      state &= ~non_blocking;

      detail::clear_last_error();
      result = detail::error_wrapper(::close(sockfd), ec);
    }
  }
  if(result == 0) { ec = std::error_code(); }
  return result;
}

int shutdown(socket_type sockfd, int what, std::error_code& ec)
{
  if(sockfd == invalid_socket)
  {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  detail::clear_last_error();
  int result = detail::error_wrapper(::shutdown(sockfd, what), ec);
  if(result == 0) { ec = std::error_code(); }
  return result;
}

int connect(socket_type sockfd, const socket_addr_type* addr, std::size_t addrlen,
            std::error_code& ec)
{
  if(sockfd == invalid_socket)
  {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  detail::clear_last_error();
  int result =
    detail::error_wrapper(detail::call_connect(&msghdr::msg_namelen, sockfd, addr, addrlen), ec);
  if(result) { ec = std::error_code(); }
  return result;
}

int listen(socket_type sockfd, int backlog, std::error_code& ec)
{
  if(sockfd == invalid_socket)
  {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  detail::clear_last_error();
  int result = detail::error_wrapper(::listen(sockfd, backlog), ec);
  if(result == 0) { ec = std::error_code(); }
  return result;
}

ssize_t recv(socket_type sockfd, buffer* bufs, std::size_t count, int flags, std::error_code& ec)
{
  detail::clear_last_error();
  msghdr msg = msghdr();
  msg.msg_iov = bufs;
  msg.msg_iovlen = static_cast<int>(count);
  int result = detail::error_wrapper(::recvmsg(sockfd, &msg, flags), ec);
  if(result >= 0) { ec = std::error_code(); }
  return result;
}

ssize_t send(socket_type sockfd, const buffer* bufs, std::size_t count, int flags,
             std::error_code& ec)
{
  detail::clear_last_error();
  msghdr msg = msghdr();
  msg.msg_iov = const_cast<buffer*>(bufs);
  msg.msg_iovlen = static_cast<int>(count);
  flags |= MSG_NOSIGNAL;
  int result = detail::error_wrapper(::sendmsg(sockfd, &msg, flags), ec);
  if(result >= 0) { ec = std::error_code(); }
  return result;
}

int setsockopt(socket_type sockfd, state_type& state, int level, int optname, const void* optval,
               std::size_t optlen, std::error_code& ec)
{
  if(sockfd == invalid_socket)
  {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  if(level == custom_socket_option_level && optname == always_fail_option)
  {
    ec = std::make_error_code(std::errc::invalid_argument);
    return socket_error_retval;
  }

  if(level == custom_socket_option_level && optname == enable_connection_aborted_option)
  {
    if(optlen != sizeof(int))
    {
      ec = std::make_error_code(std::errc::invalid_argument);
      return socket_error_retval;
    }

    if(*static_cast<const int*>(optval)) { state |= enable_connection_aborted; }
    else
    {
      state |= ~enable_connection_aborted;
    }
    ec = std::error_code();
    return 0;
  }

  if(level == SOL_SOCKET && optname == SO_LINGER) { state |= user_set_linger; }

  detail::clear_last_error();
  int result = detail::error_wrapper(detail::call_setsockopt(&msghdr::msg_name, sockfd, level,
                                                             optname, optval, optlen),
                                     ec);
  if(result == 0) { ec = std::error_code(); }
  return result;
}

int getsockopt(socket_type sockfd, state_type state, int level, int optname, void* optval,
               std::size_t* optlen, std::error_code& ec)
{
  if(sockfd == invalid_socket)
  {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  if(level == custom_socket_option_level && optname == always_fail_option)
  {
    ec = std::make_error_code(std::errc::invalid_argument);
    return socket_error_retval;
  }

  if(level == custom_socket_option_level && optname == enable_connection_aborted_option)
  {
    if(*optlen != sizeof(int))
    {
      ec = std::make_error_code(std::errc::invalid_argument);
      return socket_error_retval;
    }

    *static_cast<int*>(optval) = (state & enable_connection_aborted) ? 1 : 0;
    ec = std::error_code();
    return 0;
  }

  detail::clear_last_error();
  int result = detail::error_wrapper(detail::call_getsockopt(&msghdr::msg_namelen, sockfd, level,
                                                             optname, optval, optlen),
                                     ec);

  if(result == 0 && level == SOL_SOCKET && *optlen == sizeof(int) &&
     (optname == SO_SNDBUF || optname == SO_RCVBUF))
  { *static_cast<int*>(optval) /= 2; }

  if(result == 0) { ec = std::error_code(); }
  return result;
}

int getpeername(socket_type sockfd, socket_addr_type* addr, std::size_t* addrlen, bool /*cached*/,
                std::error_code& ec)
{
  if(sockfd == invalid_socket)
  {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  detail::clear_last_error();
  int result =
    detail::error_wrapper(detail::call_getpeername(&msghdr::msg_namelen, sockfd, addr, addrlen),
                          ec);
  if(result == 0) { ec = std::error_code(); }
  return result;
}

int getsockname(socket_type sockfd, socket_addr_type* addr, std::size_t* addrlen,
                std::error_code& ec)
{
  if(sockfd == invalid_socket)
  {
    ec = std::make_error_code(std::errc::bad_file_descriptor);
    return socket_error_retval;
  }

  detail::clear_last_error();
  int result =
    detail::error_wrapper(detail::call_getsockname(&msghdr::msg_namelen, sockfd, addr, addrlen),
                          ec);
  if(result == 0) ec = std::error_code();
  return result;
}

const char* inet_ntop(int af, const void* src, char* dest, std::size_t length,
                      unsigned long scope_id, std::error_code& ec)
{
  detail::clear_last_error();
  const char* result =
    detail::error_wrapper(::inet_ntop(af, src, dest, static_cast<int>(length)), ec);
  if(result == 0 && !ec) { ec = std::make_error_code(std::errc::invalid_argument); }
  return result;
}

int inet_pton(int af, const char* src, void* dest, unsigned long* /*scope_id*/, std::error_code& ec)
{
  detail::clear_last_error();

  int result = detail::error_wrapper(::inet_pton(af, src, dest), ec);
  if(result <= 0 && !ec) { ec = std::make_error_code(std::errc::invalid_argument); }
  return result;
}
}  // namespace boost::asio::detail::socket_ops