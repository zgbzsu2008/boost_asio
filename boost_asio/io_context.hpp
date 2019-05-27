#ifndef BOOST_IO_CONTEXT_HPP
#define BOOST_IO_CONTEXT_HPP

#include <mutex>
#include <system_error>

#include "execution_context.hpp"
#include "scheduler.hpp"
#include "service_registry.hpp"

namespace boost {
namespace asio {

class io_context : public execution_context
{
 public:
  class service;
  using impl_type = detail::scheduler;

  io_context();
  explicit io_context(int concurrency_hint);
  ~io_context();

  std::size_t run();

 private:
  template <typename Service>
  friend Service& use_service(io_context& ioc);
  impl_type& add_impl(impl_type* impl);
  impl_type& impl_;
};

class io_context::service : public execution_context::service
{
 public:
  service(io_context& owner) : execution_context::service(owner) {}
  virtual ~service() {}

  static const std::string key() { return "io_context::service"; }

  io_context& get_io_context() { return static_cast<io_context&>(context()); }

 private:
  virtual void shutdown() {}
};

namespace detail {

template <typename Type>
class io_context_service_base : public io_context::service
{
 public:
  io_context_service_base(io_context& io) : io_context::service(io) {}

  static const std::string& key() { return typeid(Type).name(); }
};

}  // namespace detail

template <typename Service>
Service& use_service(execution_context& e)
{
  return e.service_registry_->template use_service<Service>();
}

template <typename Service, typename... Args>
Service& make_service(execution_context& e, Args&&... args)
{
  std::unique_ptr<Service> service(new Service(e, std::forward<Args>(args)...));
  e.service_registry_->template add_service<Service>(service.get());
  Service& result = *service.release();
  return result;
}

template <typename Service>
void add_service(execution_context& e, Service* new_service)
{
  e.service_registry_->template add_service<Service>(new_service);
}

template <typename Service>
bool has_service(execution_context& e)
{
  return e.service_registry_->template has_service<Service>();
}

template <typename Service>
Service& use_service(io_context& ioc)
{
  return ioc.service_registry_->template use_service<Service>(ioc);
}

template <>
inline detail::scheduler& use_service<detail::scheduler>(io_context& ioc)
{
  return ioc.impl_;
}

}  // namespace asio
}  // namespace boost

#endif