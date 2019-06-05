#ifndef BOOST_ASIO_STREAM_SOCKET_SERVICE_HPP
#define BOOST_ASIO_STREAM_SOCKET_SERVICE_HPP

#include "io_context.hpp"

namespace boost::asio {
template <typename Protocol>
class stream_socket_service : public detail::service_base<stream_socket_service<Protocol>>
{
 public:
  using protocol_type = Protocol;
  using endpoint_type = typename Protocol::endpoint;
  using service_impl_type = detail::reactive_socket_service<Protocol>;
};
}  // namespace boost::asio
#endif
