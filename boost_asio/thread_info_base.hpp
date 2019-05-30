#ifndef BOOST_ASIO_DETAIL_THREAD_INFO_BASE_HPP
#define BOOST_ASIO_DETAIL_THREAD_INFO_BASE_HPP

#include <memory>
#include <type_traits>

#include "noncopyable.hpp"

namespace boost::asio::detail
{
class thread_info_base : private noncopyable
{
public:
    thread_info_base() : in_use_(false) {}

    static void* allocate(thread_info_base* this_thread, std::size_t size)
    {
        if(this_thread && !this_thread->in_use_ && (size < sizeof(storage_)))
        {
            this_thread->in_use_ = true;
            return &this_thread->storage_;
        }
        else
        {
            return ::operator new(size);
        }
    }

    static void deallocate(thread_info_base* this_thread, void* pointer,
                           std::size_t size)
    {
        if(this_thread && pointer == &this_thread->storage_ &&
           (size < sizeof(storage_)))
        {
            this_thread->in_use_ = false;
        }
        else
        {
            ::operator delete(pointer);
        }
    }

private:
    std::aligned_storage<1024>::type storage_;
    bool in_use_;
};
}  // namespace boost::asio::detail
#endif