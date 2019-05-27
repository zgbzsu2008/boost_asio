#ifndef BOOST_ASIO_SERVICE_HPP
#define BOOST_ASIO_SERVICE_HPP

#include <string>

#include "execution_context.hpp"
#include "io_context.hpp"
#include "noncopyable.hpp"

namespace boost {
namespace asio {

class execution_context::service : private detail::noncopyable {
 public:
  service(execution_context& owner) : owner_(owner) {}
  virtual ~service() {}

  static const std::string& key() { return "service"; }

  execution_context& context() { return owner_; }

 private:
  virtual void shutdown() = 0;

  friend class detail::service_registry;

  execution_context& owner_;
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
class execution_context_service_base : public execution_context::service {
 public:
  execution_context_service_base(execution_context& e)
      : execution_context::service(e) {}

  static const std::string& key() { return typeid(Type).name(); }
};

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
