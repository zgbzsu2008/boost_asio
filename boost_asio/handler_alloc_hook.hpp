#ifndef BOOST_ASIO_HANDLER_ALLOC_HOOK_HPP
#define BOOST_ASIO_HANDLER_ALLOC_HOOK_HPP

namespace boost::asio {

void* asio_handler_allocate(std::size_t size, ...) { return ::operator new(size); }

void asio_handler_deallocate(void* pointer, std::size_t size, ...) { ::operator delete(pointer); }

}  // namespace boost::asio
#endif
