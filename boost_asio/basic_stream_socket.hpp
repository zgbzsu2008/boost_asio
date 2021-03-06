#ifndef BOOST_ASIO_BASIC_STREAM_SOCKET_HPP
#define BOOST_ASIO_BASIC_STREAM_SOCKET_HPP

#include "basic_socket.hpp"
#include "stream_socket_service.hpp"

namespace boost::asio {
template <typename Protocol, typename Service = stream_socket_service<Protocol>>
class basic_stream_socket : public basic_socket<Protocol, Service>
{};
}  // namespace boost::asio
#endif
