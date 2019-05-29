#include <iostream>
#include <chrono>

#include "io_context.hpp"
#include "thread_group.hpp"
#include "post.hpp"
#include "epoll_reactor.hpp"
#include "service_registry_helpers.hpp"

using namespace boost::asio;

void init_task(io_context& ioc){ 
  detail::epoll_reactor& reactor = use_service<detail::epoll_reactor>(ioc);
  reactor.init_task();
}

void print(int idx)
{
  std::cout << idx << "\ttid = " << std::this_thread::get_id() << std::endl;
}

int main()
{
  io_context ioc;
  detail::thread_group threads(std::bind(&io_context::run, std::ref(ioc)), 4);

  std::this_thread::sleep_for(std::chrono::seconds(1));

  post(ioc, std::bind(init_task, std::ref(ioc)));

  for (int i = 0; i < 100; ++i) {
    post(ioc, std::bind(print, i));
  }
  ioc.run();
  return 0;
}