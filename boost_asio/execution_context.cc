#include "execution_context.hpp"
#include "service_registry.hpp"

namespace boost {
namespace asio {

execution_context::execution_context()
    : service_registry_(new detail::service_registry(*this)) {}

execution_context::~execution_context() {
  this->shutdown();
  this->destroy();
}

void execution_context::shutdown() { service_registry_->shutdown(); }

void execution_context::destroy() { service_registry_->destroy(); }

}  // namespace asio
}  // namespace boost