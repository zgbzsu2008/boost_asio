#ifndef BOOST_ASIO_BASIC_IO_OBJECT_HPP
#define BOOST_ASIO_BASIC_IO_OBJECT_HPP

#include "io_context.hpp"
#include "service_registry_helpers.hpp"

namespace boost::asio {
template <typename IoObjectService>
class basic_io_object
{
 public:
  using service_type = IoObjectService;
  using implementation_type = typename service_type::implementation_type;
  using executor_type = io_context::executor_type;

  executor_type get_executor() noexcept { return service_.get_io_context().get_executor(); }

  explicit basic_io_object(io_context& ctx) : service_(use_service<IoObjectService>(ctx))
  {
    service_.construct(implementation_);
  }

  ~basic_io_object() { service_.destroy(implementation_); }

  service_type& get_service() { return *service_; }
  const service_type& get_service() const { return *service_; }

  implementation_type& get_implementation() { return implementation_; }
  const implementation_type& get_implementation() const { return implementation_; }

 private:
  basic_io_object(const basic_io_object&) = delete;
  void operator=(const basic_io_object&) = delete;

  IoObjectService* service_;
  implementation_type implementation_;
};
}  // namespace boost::asio
#endif
