#ifndef BOOST_ASIO_DETAIL_CALL_STACK_HPP
#define BOOST_ASIO_DETAIL_CALL_STACK_HPP

#include <memory>
#include <thread>

#include "noncopyable.hpp"

namespace boost::asio::detail
{
template <typename Key, typename Value = unsigned char> class call_stack
{
public:
    class context : private noncopyable
    {
    public:
        explicit context(Key* k) : key_(k), next_(call_stack::top_)
        {
            value_ = reinterpret_cast<unsigned char*>(this);
            call_stack::top_ = this;
        }

        context(Key* k, Value& v) : key_(k), value_(&v), next_(call_stack::top_)
        {
            call_stack::top_ = this;
        }

        ~context() { call_stack::top_ = next_; }

        Value* next_by_key() const
        {
            for(context* elem = next_; elem != 0; elem = elem->next_)
            {
                if(elem->key_ == key_)
                {
                    return elem->value_;
                }
            }
            return 0;
        }

    private:
        friend class call_stack;
        Key* key_;
        Value* value_;
        context* next_;
    };

public:
    friend class context;
    static Value* contains(Key* k)
    {
        for(context* elem = top_; elem != 0; elem = elem->next_)
        {
            if(elem->key_ == k)
            {
                return elem->value_;
            }
        }
        return 0;
    }

    static Value* top() { return top_ ? top_->value_ : 0; }

private:
    static thread_local context* top_;
};

template <typename Key, typename Value>
thread_local
    typename call_stack<Key, Value>::context* call_stack<Key, Value>::top_ = 0;
}  // namespace boost::asio::detail
#endif
