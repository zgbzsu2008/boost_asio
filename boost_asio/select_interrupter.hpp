#ifndef BOOST_ASIO_DETAIL_SELECT_INTERRUPTER_HPP
#define BOOST_ASIO_DETAIL_SELECT_INTERRUPTER_HPP

namespace boost {
namespace asio {
namespace detail {

class select_interrupter
{
 public:
  select_interrupter();
  ~select_interrupter();

  void recreate();
  void interrupt();
  bool reset();

  int fd() const { return fd_; }

 private:
  void open_descriptors();
  void close_descriptors();

  int fd_;
};

}  // namespace detail
}  // namespace asio
}  // namespace boost
#endif