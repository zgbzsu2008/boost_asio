#ifndef BOOST_ASIO_DETAIL_REACTIVE_SOCKET_SERVICE_HPP
#define BOOST_ASIO_DETAIL_REACTIVE_SOCKET_SERVICE_HPP

#include "io_context.hpp"

namespace boost::asio::detail
{
template <typename Protocol>
class reactive_socket_service : public service_base<reactive_socket_service<Protocol>>,
                                public reactive_socket_service_base
{
 public:
  using protocol_type = Protocol;
  using endpoint_type = typename Protocol::endpoint;

  struct
};
}  // namespace boost::asio::detail
#endif
