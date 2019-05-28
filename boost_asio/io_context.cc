#include "io_context.hpp"
#include "service_registry.hpp"

#include <iostream>
#include <memory>

namespace boost {
namespace asio {

io_context::impl_type& io_context::add_impl(io_context::impl_type* impl)
{
  std::unique_ptr<impl_type> ptr(impl);
  add_service<impl_type>(*this, ptr.get());
  return *ptr.release();
}

io_context::io_context() : impl_(add_impl(new impl_type(*this, 2))) {}

io_context::io_context(int concurrency_hint)
    : impl_(add_impl(new impl_type(*this, concurrency_hint)))
{}

io_context::~io_context() {}

std::size_t io_context::run()
{
  std::error_code ec;
  auto n = impl_.run(ec);
  if (ec) {
    throw ec;
  }
  return n;
}

}  // namespace asio
}  // namespace boost