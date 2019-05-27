#ifndef BOOST_ASIO_DETAIL_SERVICE_REGISTRY_HPP
#define BOOST_ASIO_DETAIL_SERVICE_REGISTRY_HPP

#include "execution_context.hpp"

namespace boost {
namespace asio {
namespace detail {

class service_registry{
 public:
  service_registry(execution_context& context);

  void shutdown();
  void destroy();

 private:

}
}
}
}
#endif