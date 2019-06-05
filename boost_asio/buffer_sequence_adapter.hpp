#ifndef BOOST_ASIO_DETAIL_BUFFER_SEQUENCE_ADAPTER_HPP
#define BOOST_ASIO_DETAIL_BUFFER_SEQUENCE_ADAPTER_HPP

#include "buffer.hpp"
#include "socket_types.hpp"

namespace boost::asio::detail {
class buffer_sequence_adapter_base
{
 public:
  enum
  {
    max_buffers = 64 < max_iov_len ? 64 : max_iov_len
  };
  using native_buffer_type = iovec;

  static void init_iov_base(void*& base, void* addr) { base = addr; }
  template <typename T>
  static void init_iov_base(T& base, void* addr)
  {
    base = static_cast<T>(addr);
  }

  static void init_native_buffer(iovec& iov, boost::asio::mutable_buffer& buffer)
  {
    init_iov_base(iov.iov_base, buffer.data());
    iov.iov_len = buffer.size();
  }

  static void init_native_buffer(iovec& iov, boost::asio::const_buffer& buffer)
  {
    init_iov_base(iov.iov_base, const_cast<void*>(buffer.data()));
    iov.iov_len = buffer.size();
  }
};

template <typename Buffer, typename Buffers>
class buffer_sequence_adapter : private buffer_sequence_adapter_base
{
 public:
  explicit buffer_sequence_adapter(const Buffers& buffer_sequence&)
      : count_(0), total_buffer_size_(0)
  {
    init(std::addressof(buffer_sequence), std::addressof(buffer_sequence) + 1);
  }

  native_buffer_type* buffers() { return buffers_; }
  std::size_t count() const { return count_; }
  std::size_t total_size() const { return total_buffer_size_; }
  bool all_empty() const { return total_buffer_size_ == 0; }

  static bool all_empty(const Buffer& buffer_sequence) {}

  static void validate(const Buffers& buffer_sequence) {}

  static Buffer first(const Buffers& buffer_sequence) {}

 private:
  template <typename Iterator>
  void init(Iterator begin, Iterator end)
  {
    for (Iterator iter = begin; iter != end && count_ < max_buffers; ++iter, ++count_) {
      Buffer buffer(*iter);
      init_native_buffer(buffers_[count_], buffer);
      total_buffer_size_ += buffer.size();
    }
  }

  template <typename Iterator>
  static bool all_empty(Iterator begin, Iterator end)
  {
    for (Iterator iter = begin, std::size_t i = 0; iter != end && i < max_buffers; ++iter, ++i) {
      if (Buffer(*iter).size() > 0) {
        return false;
      }
    }
    return true;
  }

  template <typename Iterator>
  static void validate(Iterator begin, Iterator end)
  {
    for (Iterator iter = begin; iter != end; ++iter) {
      Buffer buffer(*iter);
      buffer.data();
    }
  }

  template <typename Iterator>
  static Buffer first(Iterator begin, Iterator end)
  {
    for (Iterator iter = begin; iter != end; ++iter) {
      Buffer buffer(*iter);
      if (buffer.size() != 0) {
        return buffer;
      }
    }
    return Buffer();
  }

  native_buffer_type buffers_[max_buffers];
  std::size_t count_;
  std::size_t total_buffer_size_;
};
}  // namespace boost::asio::detail
#endif