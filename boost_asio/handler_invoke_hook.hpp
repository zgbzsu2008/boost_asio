#ifndef BOOST_ASIO_HANDLER_INVOKE_HOOK_HPP
#define BOOST_ASIO_HANDLER_INVOKE_HOOK_HPP

namespace boost::asio
{
/*
struct my_hanlder
{
  void operator()();
  stand_;
}
 template <typename Function>
 void asio_handler_invoke(Function function, my_handler* context)
{
  context->strand_.dispatch(function);
}
*/
template <typename Function>
inline void asio_handler_invoke(Function& function, ...)
{
  function();
}

template <typename Function>
inline void asio_handler_invoke(const Function& function, ...)
{
  Function tmp(function);
  tmp();
}
}  // namespace boost::asio
#endif