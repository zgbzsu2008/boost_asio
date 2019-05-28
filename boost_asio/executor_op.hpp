#ifndef BOOST_ASIO_DETAIL_EXECUTOR_OP_HPP22
#define BOOST_ASIO_DETAIL_EXECUTOR_OP_HPP

#include <functional>

#include "fenced_block.hpp"
#include "recycling_allocator.hpp"
#include "scheduler_operation.hpp"
#include "thread_context.hpp"

namespace boost {
namespace asio {
namespace detail {

template <typename Handler = std::function<void>,
          typename Alloc = recycling_allocator<void>,
          typename Operation = scheduler_operation>
class executor_op : public Operation
{
 public:
  struct ptr
  {
    using op = executor_op;
    const Alloc* a;
    void* v;
    op* p;
    ~ptr() { reset(); }

    static op* allocate(const Alloc& alloc)
    {
      using alloc_type = typename get_recycling_allocator<Alloc>::type;
      using alloc_op = typename rebind_alloc<alloc_type, op>::type;
      alloc_op a1(get_recycling_allocator<Alloc>::get(alloc));
      return a1.allocate(1);
    }
    void reset()
    {
      if (p) {
        p->~op();
        p = 0;
      }
      if (v) {
        using alloc_type = typename get_recycling_allocator<Alloc>::type;
        using alloc_op = typename rebind_alloc<alloc_type, op>::type;
        alloc_op a1(get_recycling_allocator<Alloc>::get(*a));
        a1.deallocate(static_cast<op*>(v), 1);
        v = 0;
      }
    }
  };

  template <typename Function>
  executor_op(const Function& func, Alloc a)
      : scheduler_operation(&executor_op::do_complete),
        handler_(func),
        alloc_(a)
  {}

 private:
  static void do_complete(void* owner, scheduler_operation* base,
                          const std::error_code&, std::size_t)
  {
    executor_op* e = static_cast<executor_op*>(base);
    Alloc a(e->alloc_);
    ptr p = {std::addressof(a), e, e};
    Handler copy_handler(e->handler_);
    p.reset();
    if (owner) {
      fenced_block b(fenced_block::half);
      copy_handler();
    }
  }

  Handler handler_;
  Alloc alloc_;
};

}  // namespace detail
}  // namespace asio
}  // namespace boost
#endif