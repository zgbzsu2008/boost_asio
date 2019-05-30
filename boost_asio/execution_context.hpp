#ifndef BOOST_ASIO_EXECUTION_CONTEXT_HPP
#define BOOST_ASIO_EXECUTION_CONTEXT_HPP

#include <iostream>
#include <memory>
#include <string>

#include "noncopyable.hpp"

namespace boost::asio
{
namespace detail
{
class service_registry;
}
class execution_context : private detail::noncopyable
{
 public:
  class service;

  execution_context();
  virtual ~execution_context();

  void shutdown();
  void destroy();

  template <typename Service> friend Service& use_service(execution_context& e);

  template <typename Service, typename... Args>
  friend Service& make_service(execution_context& e, Args&&... args);

  template <typename Service>
  friend void add_service(execution_context& e, Service* new_service);

  template <typename Service> friend bool has_service(execution_context& e);

 protected:
  std::unique_ptr<detail::service_registry> service_registry_;
};

class execution_context::service : private detail::noncopyable
{
 public:
  service(execution_context& owner) : owner_(owner) {}
  virtual ~service() {}

  static std::string key() { return "execution_context::service"; }

  execution_context& context() { return owner_; }

 private:
  virtual void shutdown() = 0;

  friend class detail::service_registry;

  execution_context& owner_;
};

namespace detail
{
template <typename Type>
class execution_context_service_base : public execution_context::service
{
 public:
  execution_context_service_base(execution_context& e)
    : execution_context::service(e)
  {
  }

  static std::string key() { return typeid(Type).name(); }
};
}  // namespace detail
}  // namespace boost::asio
#endif
