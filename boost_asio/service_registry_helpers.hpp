#ifndef BOOST_ASIO_SERVICE_REGISTRY_HELPERS_HPP
#define BOOST_ASIO_SERVICE_REGISTRY_HELPERS_HPP

#include "execution_context.hpp"
#include "io_context.hpp"
#include "service_registry.hpp"

namespace boost::asio
{
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
}  // namespace boost::asio
#endif
