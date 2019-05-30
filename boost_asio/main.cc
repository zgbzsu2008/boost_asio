#include <chrono>
#include <iostream>
#include <iomanip>

#include "epoll_reactor.hpp"
#include "io_context.hpp"
#include "post.hpp"
#include "service_registry_helpers.hpp"
#include "thread_group.hpp"

using namespace boost::asio;

void init_task(io_context& ioc)
{
  detail::epoll_reactor& reactor = use_service<detail::epoll_reactor>(ioc);
  reactor.init_task();
}

void print(int idx)
{
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::cout << std::setw(4) << idx << " tid = " << std::this_thread::get_id()
            << std::endl;
}

int main()
{
  io_context ioc;
  executor_work_guard work = make_work_guard(ioc);

  detail::thread_group threads(std::bind(&io_context::run, std::ref(ioc)),
                               std::thread::hardware_concurrency());
  post(ioc, std::bind(init_task, std::ref(ioc)));

  std::this_thread::sleep_for(std::chrono::seconds(1));
  for(int i = 0; i < 100; ++i)
  {
    post(ioc, std::bind(print, i));
  }

  ioc.run();
  return 0;
}