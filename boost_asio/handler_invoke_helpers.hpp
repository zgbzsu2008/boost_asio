#ifndef BOOST_ASIO_DETAIL_HANDLER_INVOKE_HELPERS_HPP
#define BOOST_ASIO_DETAIL_HANDLER_INVOKE_HELPERS_HPP

#include "handler_invoke_hook.hpp"

namespace boost_asio_handler_invoke_helpers
{
template <typename Function, typename Context>
inline void invoke(Function& function, Context& context)
{
  using boost::asio::asio_handler_invoke;
  asio_handler_invoke(function, std::addressof(context));
}

template <typename Function, typename Context>
inline void invoke(const Function& function, Context& context)
{
  using boost::asio::asio_handler_invoke;
  asio_handler_invoke(function, std::addressof(context));
}
}  // namespace boost_asio_handler_invoke_helpers
#endif