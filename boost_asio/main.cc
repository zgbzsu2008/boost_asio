#include "io_context.hpp"

using namespace boost::asio;

int main()
{
  io_context io;
  io.run();
  return 0;
}