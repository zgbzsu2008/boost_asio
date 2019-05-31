#ifndef BOOST_ASIO_DETAIL_REACTIVE_SOCKET_SERVICE_BASE_HPP
#define BOOST_ASIO_DETAIL_REACTIVE_SOCKET_SERVICE_BASE_HPP

#include "socket_types.hpp"
#include "socket_ops.hpp"
#include "epoll_reactor.hpp"

namespace boost::asio::detail
{
class reactive_socket_service_base
{
 public:
  using native_handle_type = socket_type;

  struct base_implementation_type
  {
    socket_type socket_;
    socket_ops::state_type state_;
    epoll_reactor::pre_descriptor_data reactor_data_;
  };

  reactive_socket_service_base(boost::asio::io_context& io_context);

  void base_shutdown();

  void construct(base_implementation_type& impl);

  void destroy(base_implementation_type& impl);

  bool is_open(const base_implementation_type& impl) const
  {
    return impl.socket_ != invalid_socket;
  }

  std::error_code close(base_implementation_type& impl, std::error_code& ec);

  native_handle_type native_handle(base_implementation_type& impl) { return impl.socket_; }

  std::error_code cancel(base_implementation_type& impl, std::error_code& ec);

  std::error_code listen(base_implementation_type& impl, int backlog, std::error_code& ec)
  {
    socket_ops::listen(impl.socket_, backlog, ec);
    return ec;
  }

 protected:
  std::error_code do_open(base_implementation_type& impl, int af, int type, int protocol,
                          std::error_code& ec);
  void start_op(base_implementation_type& impl, int op_type, reactor_op* op, bool is_continuation,
                bool peer_is_open);

  void start_accept_op(base_implementation_type& impl, reactor_op* op, bool is_continuation,
                       bool peer_is_open);

  void start_connect_op(base_implementation_type& impl, reactor_op* op, bool is_continuation,
                        const socket_addr_type* addr, size_t addrlen);
  io_context& io_context_;
  epoll_reactor& reactor_;
};
}  // namespace boost::asio::detail
#endif
