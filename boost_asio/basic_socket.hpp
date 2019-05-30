#ifndef BOOST_ASIO_BASIC_SOCKET_HPP
#define BOOST_ASIO_BASIC_SOCKET_HPP

#include "basic_io_object.hpp"
#include "socket_base.hpp"

namespace boost::asio
{
template <typename Protocol, typename Service>
class basic_socket : public basic_io_object<Service>, public socket_base
{
};
}  // namespace boost::asio
#endif
