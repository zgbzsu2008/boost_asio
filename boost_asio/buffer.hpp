#ifndef BOOST_ASIO_BUFFER_HPP
#define BOOST_ASIO_BUFFER_HPP

#include <iostream>

namespace boost::asio {
class mutable_buffer
{
 public:
  mutable_buffer() : data_(0), size_(0) {}
  mutable_buffer(void* data, std::size_t size) : data_(data), size_(size) {}

  void* data() const { return data_; }
  std::size_t size() const { return size_; }

  mutable_buffer& operator+=(std::size_t n)
  {
    std::size_t offset = n < size_ ? n : size_;
    data_ = static_cast<char*>(data_) + offset;
    size_ -= offset;
    return *this;
  }

 private:
  void* data_;
  std::size_t size_;
};

class const_buffer
{
 public:
  const_buffer() : data_(0), size_(0) {}
  const_buffer(const void* data, std::size_t size) : data_(data), size_(size) {}

  const void* data() const { return data_; }
  std::size_t size() const { return size_; }

  const_buffer& operator+=(std::size_t n)
  {
    std::size_t offset = n < size_ ? n : size_;
    data_ = static_cast<const char*>(data_) + offset;
    size_ -= offset;
    return *this;
  }

 private:
  const void* data_;
  std::size_t size_;
};
}  // namespace boost::asio
#endif