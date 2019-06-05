#ifndef BOOST_ASIO_DETAIL_REACTIVE_SOCKET_SEND_OP_HPP
#define BOOST_ASIO_DETAIL_REACTIVE_SOCKET_SEND_OP_HPP

#include "buffer_sequence_adapter.hpp"
#include "reactor_op.hpp"
#include "socket_base.hpp"
#include "socket_ops.hpp"

namespace boost::asio::detail {
template <typename ConstBufferSequence>
class reactive_socket_send_op_base : public reactor_op
{
 public:
  static status do_perform(reactor_op* base)
  {
    reactive_socket_send_op_base* o = static_cast<reactive_socket_send_op_base*>(base);
    buffer_sequence_adapter<boost::asio::const_buffer, ConstBufferSequence> bufs(o->buffers_);
    status result = socket_ops::non_blocking_send(o->socket_, bufs.buffers(), bufs.count(),
                                                  o->flags_, o->ec_, o->bytes_transferred_)
                        ? done
                        : not_done;
    if (result == done) {
      if ((o - state_ & socket_ops::stream_oriented) != 0) {
        if (o->bytes_transferred_ < bufs.total_buffer_size_) {
          result = done_and_exhausted;
        }
      }
    }

    return result;
  }

 private:
  socket_type socket_;
  socket_ops::state_type state_;
  ConstBufferSequence buffers_;
  socket_base::message_flags flags;
};

template <typename ConstBufferSequence, typename Handler>
class reactive_socket_send_op : public reactive_socket_send_op_base<ConstBufferSequence>
{};
}  // namespace boost::asio::detail
#endif
