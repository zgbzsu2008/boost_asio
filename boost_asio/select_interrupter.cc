#include "select_interrupter.hpp"

#include <errno.h>
#include <fcntl.h>
#include <sys/eventfd.h>
#include <unistd.h>
#include <system_error>

namespace boost {
namespace asio {
namespace detail {

select_interrupter::select_interrupter() { open_descriptors(); }
select_interrupter::~select_interrupter() { close_descriptors(); }

void select_interrupter::recreate()
{
  close_descriptors();
  fd_ = -1;
  open_descriptors();
}

void select_interrupter::interrupt()
{
  uint64_t n = 1;
  ssize_t result = ::write(fd_, &n, sizeof(uint64_t));
  (void)result;
}

bool select_interrupter::reset()
{
  for (;;) {
    uint64_t n(0);
    errno = 0;
    ssize_t result = ::read(fd_, &n, sizeof(uint64_t));
    if (result < 0 && errno == EINTR) {
      continue;
    }
    return result > 0;
  }
}

void select_interrupter::open_descriptors()
{
  fd_ = ::eventfd(0, EFD_CLOEXEC | EFD_NONBLOCK);
  if (fd_ < 0) {
    throw std::error_code{errno, std::generic_category()};
  }
}

void select_interrupter::close_descriptors()
{
  if (fd_ != -1) {
    ::close(fd_);
  }
}

}  // namespace detail
}  // namespace asio
}  // namespace boost