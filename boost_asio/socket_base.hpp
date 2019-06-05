#ifndef BOOST_ASIO_SOCKET_BASE_HPP
#define BOOST_ASIO_SOCKET_BASE_HPP

namespace boost::asio {
class socket_base
{
 public:
  enum shutdown_type
  {
    shutdown_receive = 0x01,
    shutdown_send = 0x02,
    shutdown_both = 0x03
  };

  enum wait_type
  {
    wait_read = 0x01,
    wait_write = 0x02,
    wait_error = 0x04
  };

  using message_flags = int;
  static constexpr int message_peek = 0x02;
  static constexpr int message_out_of_band = 0x01;
  static constexpr int message_do_not_route = 0x04;
  static constexpr int message_end_of_record = 0;
};
}  // namespace boost::asio
#endif
