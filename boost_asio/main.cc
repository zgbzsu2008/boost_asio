#include "io_context.hpp"
#include "thread_guard.hpp"

using namespace boost::asio;

int main()
{
  io_context io;
  detail::thread_guard threads(std::bind(&io_context::run, std::ref(io)), 1);

  io.run();
  return 0;
}