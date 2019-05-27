#ifndef BOOST_ASIO_DETAIL_SCHEDULER_THREAD_INFO_HPP
#define BOOST_ASIO_DETAIL_SCHEDULER_THREAD_INFO_HPP

#include "thread_info_base.hpp"

namespace boost {
namespace asio {
namespace detail {
class scheduler;
class scheduler_operation;
struct scheduler_thread_info : public thread_info_base {
  op_queue<scheduler_operation> private_op_queue;
  long private_outstanding_work;
};
}
}

}  // namespace boost::asio::detail
#endif  // !BOOST_ASIO_DETAIL_SCHEDULER_THREAD_INFO_HPP
