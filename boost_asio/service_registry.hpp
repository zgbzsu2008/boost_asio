#ifndef BOOST_ASIO_DETAIL_SERVICE_REGISTRY_HPP
#define BOOST_ASIO_DETAIL_SERVICE_REGISTRY_HPP

#include <assert.h>
#include <map>
#include <memory>
#include <mutex>
#include <string>

#include "execution_context.hpp"
#include "io_context.hpp"

namespace boost::asio::detail {
class service_registry
{
 public:
  using service = execution_context::service;

  service_registry(execution_context& context);

  void shutdown_services();
  void destroy_services();

  template <typename Service>
  Service& use_service();
  template <typename Service>
  Service& use_service(io_context& ioc);
  template <typename Service>
  void add_service(Service* new_service);
  template <typename Service>
  bool has_service();

 private:
  mutable std::mutex mutex_;
  execution_context& owner_;
  std::map<std::string, service*> services_;
};

template <typename Service>
inline Service& service_registry::use_service()
{
  std::string key = Service::key();
  std::unique_lock<std::mutex> lock(mutex_);
  if (services_.find(key) != services_.end()) {
    return static_cast<Service&>(*services_[key]);
  }

  lock.unlock();
  std::unique_ptr<Service> new_service(new Service(owner_));
  lock.lock();

  if (services_.find(key) != services_.end()) {
    return static_cast<Service&>(*services_[key]);
  }
  services_[key] = new_service.get();

  return static_cast<Service&>(*new_service.release());
}

template <typename Service>
inline Service& service_registry::use_service(io_context& ioc)
{
  std::string key = Service::key();
  std::unique_lock<std::mutex> lock(mutex_);
  if (services_.find(key) != services_.end()) {
    return static_cast<Service&>(*services_[key]);
  }

  lock.unlock();
  std::unique_ptr<Service> new_service(new Service(ioc));
  lock.lock();

  if (services_.find(key) != services_.end()) {
    return static_cast<Service&>(*services_[key]);
  }
  services_[key] = new_service.get();

  return static_cast<Service&>(*new_service.release());
}

template <typename Service>
inline void service_registry::add_service(Service* new_service)
{
  assert(&owner_ == &new_service->context());

  std::string key = Service::key();
  std::lock_guard<std::mutex> lock(mutex_);

  assert(services_.find(key) == services_.end());
  services_[key] = new_service;
}

template <typename Service>
inline bool service_registry::has_service()
{
  std::string key = Service::key();
  std::lock_guard<std::mutex> lock(mutex_);
  return services_.find(key) != services_.end();
}
}  // namespace boost::asio::detail
#endif