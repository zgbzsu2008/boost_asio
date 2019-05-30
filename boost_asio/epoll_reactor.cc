#include "epoll_reactor.hpp"

#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#include <mutex>

#include "service_registry_helpers.hpp"

namespace boost::asio::detail
{

epoll_reactor::epoll_reactor(execution_context& ctx)
  : execution_context_service_base(ctx),
    scheduler_(use_service<scheduler>(ctx)), interrupter_(),
    epoll_fd_(do_epoll_create()), shutdown_(false), mutex_()
{
    epoll_event ev = {0, {0}};
    ev.events = EPOLLIN | EPOLLERR | EPOLLET;
    ev.data.ptr = &interrupter_;
    ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, interrupter_.fd(), &ev);
    interrupter_.interrupt();
}

epoll_reactor::~epoll_reactor()
{
    if(epoll_fd_ != -1)
    {
        ::close(epoll_fd_);
    }
}

void epoll_reactor::shutdown()
{
    std::unique_lock<std::mutex> lock(mutex_);
    shutdown_ = true;
    lock.unlock();
    op_queue<operation> ops;
    for(auto it = registered_descriptors_.begin();
        it != registered_descriptors_.end(); ++it)
    {
        for(int i = 0; i < max_ops; ++i)
        {
            ops.push(it->second->op_queue_[i]);
        }
        it->second->shutdown_ = true;
        delete it->second;
    }
}

void epoll_reactor::init_task()
{
    scheduler_.init_task();
}

void epoll_reactor::run(long usec, op_queue<operation>& ops)
{
    int timeout = 0;
    if(usec)
    {
        timeout = int((usec < 0) ? -1 : ((usec - 1) / 1000 + 1));
    }

    epoll_event events[128];
    int numEvents = ::epoll_wait(epoll_fd_, events, 128, timeout);

    for(int i = 0; i < numEvents; ++i)
    {
        void* ptr = events[i].data.ptr;
        if(ptr == &interrupter_)
        {
            std::cout << "epoll_reactor::run(): interrupter fd = "
                      << interrupter_.fd() << std::endl;
        }
        else
        {
            auto descriptor_data = static_cast<pre_descriptor_data>(ptr);
            std::cout << "epoll_reactor::run(): descriptor_state fd = "
                      << descriptor_data->descriptor_ << std::endl;
            if(!ops.is_enqueued(descriptor_data))
            {
                descriptor_data->set_ready_events(events[i].events);
                ops.push(descriptor_data);
            }
            else
            {
                descriptor_data->add_ready_events(events[i].events);
            }
        }
    }
}

void epoll_reactor::interrupt()
{
    epoll_event ev = {0, {0}};
    ev.events = EPOLLIN | EPOLLERR | EPOLLET;
    ev.data.ptr = &interrupter_;
    ::epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, interrupter_.fd(), &ev);
    std::cout << "epoll_reactor::interrupt(): fd = " << interrupter_.fd()
              << std::endl;
}

void epoll_reactor::post_immediate_completion(reactor_op* op,
                                              bool is_continuation)
{
    scheduler_.post_immediate_completion(op, is_continuation);
}

int epoll_reactor::register_descriptor(socket_type descriptor,
                                       pre_descriptor_data& descriptor_data)
{
    descriptor_data = new descriptor_state(true);
    {
        std::lock_guard<std::mutex> lock(descriptor_data->mutex_);
        descriptor_data->reactor_ = this;
        descriptor_data->descriptor_ = descriptor;
        descriptor_data->shutdown_ = false;
        for(int i = 0; i < max_ops; ++i)
        {
            descriptor_data->try_speculative_[i] = true;
        }
        registered_descriptors_[descriptor] = descriptor_data;
    }
    epoll_event ev = {0, {0}};
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
    descriptor_data->registered_events_ = ev.events;
    ev.data.ptr = descriptor_data;
    int result = ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, descriptor, &ev);
    if(result != 0)
    {
        if(errno == EPERM)
        {
            descriptor_data->registered_events_ = 0;
            return 0;
        }
        return errno;
    }
    return 0;
}

int epoll_reactor::register_internal_descriptor(
    int op_type, socket_type descriptor, pre_descriptor_data& descriptor_data,
    reactor_op* op)
{
    descriptor_data = new descriptor_state(true);
    {
        std::lock_guard<std::mutex> lock(descriptor_data->mutex_);
        descriptor_data->reactor_ = this;
        descriptor_data->descriptor_ = descriptor;
        descriptor_data->shutdown_ = false;
        descriptor_data->op_queue_[op_type].push(op);
        for(int i = 0; i < max_ops; ++i)
        {
            descriptor_data->try_speculative_[i] = true;
        }
        registered_descriptors_[descriptor] = descriptor_data;
    }
    epoll_event ev = {0, {0}};
    ev.events = EPOLLIN | EPOLLERR | EPOLLHUP | EPOLLPRI | EPOLLET;
    descriptor_data->registered_events_ = ev.events;
    ev.data.ptr = descriptor_data;
    int result = ::epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, descriptor, &ev);
    if(result != 0)
    {
        return errno;
    }
    return 0;
}

void epoll_reactor::start_op(int op_type, socket_type descriptor,
                             pre_descriptor_data& descriptor_data,
                             reactor_op* op, bool is_continuation,
                             bool allow_speculative)
{
}

void epoll_reactor::cancel_ops(socket_type,
                               pre_descriptor_data& descriptor_data)
{
    if(!descriptor_data)
    {
        return;
    }
    std::unique_lock<std::mutex> lock(descriptor_data->mutex_);

    op_queue<operation> ops;
    for(int i = 0; i < max_ops; ++i)
    {
        while(reactor_op* op = descriptor_data->op_queue_[i].front())
        {
            op->ec_ = std::error_code();
            descriptor_data->op_queue_[i].pop();
            ops.push(op);
        }
    }
    lock.unlock();
    scheduler_.post_deferred_completions(ops);
}

void epoll_reactor::deregister_descriptor(socket_type descriptor,
                                          pre_descriptor_data& descriptor_data,
                                          bool closing)
{
    if(!descriptor_data)
    {
        return;
    }

    std::unique_lock<std::mutex> lock(descriptor_data->mutex_);
    if(!descriptor_data->shutdown_)
    {
        if(closing)
        {
            //
        }
        else if(descriptor_data->registered_events_ != 0)
        {
            epoll_event ev = {0, {0}};
            ::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, descriptor, &ev);
        }

        op_queue<operation> ops;
        for(int i = 0; i < max_ops; ++i)
        {
            while(reactor_op* op = descriptor_data->op_queue_[i].front())
            {
                op->ec_ = std::error_code();
                descriptor_data->op_queue_[i].pop();
                ops.push(op);
            }
        }

        descriptor_data->descriptor_ = -1;
        descriptor_data->shutdown_ = true;

        lock.unlock();
        scheduler_.post_deferred_completions(ops);
    }
    else
    {
        descriptor_data = 0;
    }
}

void epoll_reactor::deregister_internal_descriptor(
    socket_type descriptor, pre_descriptor_data& descriptor_data)
{
    if(!descriptor_data)
    {
        return;
    }

    std::unique_lock<std::mutex> lock(descriptor_data->mutex_);
    if(!descriptor_data->shutdown_)
    {
        epoll_event ev = {0, {0}};
        ::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, descriptor, &ev);

        op_queue<operation> ops;
        for(int i = 0; i < max_ops; ++i)
        {
            ops.push(descriptor_data->op_queue_[i]);
        }
        descriptor_data->descriptor_ = -1;
        descriptor_data->shutdown_ = true;

        lock.unlock();
        scheduler_.post_deferred_completions(ops);
    }
    else
    {
        descriptor_data = 0;
    }
}

int epoll_reactor::do_epoll_create()
{
    int fd = ::epoll_create1(EPOLL_CLOEXEC);
    if(fd < 0)
    {
        std::cout << "EPoller::do_epoll_create(): epoll_create1 failed."
                  << std::endl;
        std::error_code ec(errno, std::generic_category());
        throw ec;
    }
    return fd;
}

struct epoll_reactor::perform_io_cleanup_on_block_exit
{
    explicit perform_io_cleanup_on_block_exit(epoll_reactor* r)
      : reactor_(r), first_op_(0)
    {
    }
    ~perform_io_cleanup_on_block_exit()
    {
        if(first_op_)
        {
            if(!ops_.empty())
            {
                reactor_->scheduler_.post_deferred_completions(ops_);
            }
        }
        else
        {
            reactor_->scheduler_.compensating_work_started();
        }
    }
    epoll_reactor* reactor_;
    op_queue<operation> ops_;
    operation* first_op_;
};

epoll_reactor::descriptor_state::descriptor_state(bool /*locking*/)
  : operation(&epoll_reactor::descriptor_state::do_complete), mutex_()
{
}

operation* epoll_reactor::descriptor_state::perform_io(uint32_t events)
{
    mutex_.lock();
    perform_io_cleanup_on_block_exit io_cleanup(reactor_);
    std::unique_lock<std::mutex> lock(mutex_, std::adopt_lock);  // mutex locked
    static const int flag[max_ops] = {EPOLLIN, EPOLLOUT, EPOLLPRI};
    for(int j = max_ops - 1; j >= 0; --j)
    {
        if(events & (flag[j] | EPOLLERR | EPOLLHUP))
        {
            try_speculative_[j] = true;
            while(reactor_op* op = op_queue_[j].front())
            {
                if(auto status = op->perform())
                {
                    op_queue_[j].pop();
                    io_cleanup.ops_.push(op);
                    if(status == reactor_op::done_and_exhausted)
                    {
                        try_speculative_[j] = false;
                        break;
                    }
                }
                else
                {
                    break;
                }
            }
        }
    }

    io_cleanup.first_op_ = io_cleanup.ops_.front();
    io_cleanup.ops_.pop();
    return io_cleanup.first_op_;
}

void epoll_reactor::descriptor_state::do_complete(void* owner, operation* base,
                                                  const std::error_code& ec,
                                                  std::size_t bytes_transferred)
{
    if(owner)
    {
        auto data = static_cast<descriptor_state*>(base);
        uint32_t events = static_cast<uint32_t>(bytes_transferred);
        if(auto op = data->perform_io(events))
        {
            op->complete(owner, ec, 0);
        }
    }
}
}  // namespace boost::asio::detail
