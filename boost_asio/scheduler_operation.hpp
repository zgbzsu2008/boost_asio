#ifndef BOOST_ASIO_DETAIL_SCHEDULER_OPERATION_HPP
#define BOOST_ASIO_DETAIL_SCHEDULER_OPERATION_HPP

#include <functional>
#include <system_error>

#include "op_queue.hpp"

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;

namespace boost {
namespace asio {
namespace detail {

class scheduler;
class scheduler_operation
{
 public:
  using func_type = std::function<void(void*, scheduler_operation*,
                                       const std::error_code&, std::size_t)>;

  scheduler_operation(const func_type& func) : task_result_(0), func_(func) {}

  void complete(void* owner, const std::error_code& ec,
                std::size_t bytes_transferred)
  {
    if (func_) {
      func_(owner, this, ec, bytes_transferred);
    }
  }

  void destroy(void* owner, const std::error_code& ec,
               std::size_t bytes_transferred)
  {
    if (func_) {
      func_(0, this, std::error_code(), 0);
    }
  }

  friend class scheduler;
  unsigned int task_result_;

 private:
  func_type func_;
};
using operation = scheduler_operation;

}  // namespace detail
}  // namespace asio
}  // namespace boost
#endif