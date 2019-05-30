#ifndef BOOST_ASIO_DETAIL_REACTOR_OP_HPP2
#define BOOST_ASIO_DETAIL_REACTOR_OP_HPP2

#include "scheduler_operation.hpp"

namespace boost::asio::detail
{

class reactor_op : public scheduler_operation
{
public:
    enum status
    {
        not_done,
        done,
        done_and_exhausted
    };

    using perform_func_type = std::function<status(reactor_op*)>;

    std::error_code ec_;
    std::size_t bytes_transferred_;

    status perform() { return perform_func_(this); }

    reactor_op(const perform_func_type& perfom_func, const func_type& func)
      : scheduler_operation(func), bytes_transferred_(0),
        perform_func_(perfom_func)

    {
    }

private:
    perform_func_type perform_func_;
};
}  // namespace boost::asio::detail
#endif
