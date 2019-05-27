#ifndef BOOST_ASIO_EXECUTION_CONTEXT_HPP
#define BOOST_ASIO_EXECUTION_CONTEXT_HPP

#include <iostream>
#include <memory>

#include "noncopyable.hpp"

namespace boost {
namespace asio {

namespace detail {
class service_registry;
}

class execution_context : private detail::noncopyable {
 public:
  class service;

  execution_context();
  virtual ~execution_context();

  void shutdown();
  void destroy();

 private:
  std::unique_ptr<detail::service_registry> service_registry_;
};

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

namespace detail {

template <typename Type>
class execution_context_service_base : public execution_context::service {
 public:
  execution_context_service_base(execution_context& e)
      : execution_context::service(e) {}

  static const std::string& key() { return typeid(Type).name(); }
};

}  // namespace detail

}  // namespace asio
}  // namespace boost
#endif