#include "service_registry.hpp"

namespace boost::asio::detail
{
service_registry::service_registry(execution_context& context) : owner_(context)
{
}

void service_registry::shutdown_services()
{
  for(auto it : services_)
  {
    it.second->shutdown();
  }
}

void service_registry::destroy_services()
{
  for(auto& it : services_)
  {
    delete it.second;
  }
  services_.clear();
}
}  // namespace boost::asio::detail