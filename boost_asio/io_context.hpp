#ifndef BOOST_IO_CONTEXT_HPP
#define BOOST_IO_CONTEXT_HPP

#include <system_error>
#include <mutex>

#include "execution_context.hpp"
#include "scheduler.hpp"

namespace boost {
namespace asio {

class io_context : public execution_context{
 public:
  class service;
  io_context();
  ~io_context();

  std::size_t run();

 private:
  detail::scheduler& scheduler;
};

class io_context::service : public execution_context::service {
 public:
  service(io_context& owner) : execution_context::service(owner) {}
  virtual ~service() {}

  static const std::string& key() { return "io_context::service"; }

  io_context& get_io_context() { return static_cast<io_context&>(context()); }

 private:
  virtual void shutdown() {}
};

namespace detail {

template <typename Type>
class io_context_service_base : public io_context::service {
 public:
  io_context_service_base(io_context& io) : io_context::service(io) {}

  static const std::string& key() { return typeid(Type).name(); }
};

}  // namespace detail

}  // namespace asio
}  // namespace boost

#endif