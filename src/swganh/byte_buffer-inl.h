// This file is part of SWGANH which is released under the MIT license.
// See file LICENSE or go to http://swganh.com/LICENSE
#pragma once

#include <algorithm>
#include <string>
#include <stdexcept>

#include "utilities.h"

namespace swganh {

template<typename T>
ByteBuffer& ByteBuffer::write(T data) {
  write(reinterpret_cast<unsigned char*>(&data), sizeof(T));
  return *this;
}

template<typename T>
ByteBuffer& ByteBuffer::write(std::vector<T> vec) {
    std::for_each(vec.begin(), vec.end(), [this] (T data)
    {
        this->write(data);
    });
    return *this;
}

template<typename T>
ByteBuffer& ByteBuffer::writeAt(size_t offset, T data) {
  write(offset, reinterpret_cast<unsigned char*>(&data), sizeof(T));
  return *this;
}

template<typename T>
const T ByteBuffer::read(bool doSwapEndian, bool null_terminated_string) {
  T data = peek<T>(doSwapEndian);
  read_position_ += sizeof(T);
  return data;
}

template<typename T>
const T ByteBuffer::peek(bool doSwapEndian) const {
  return peekAt<T>(read_position_, doSwapEndian);
}

template<typename T>
const T ByteBuffer::peekAt(size_t offset, bool doSwapEndian) const {
  if (data_.size() < offset + sizeof(T)) {
    throw std::out_of_range("Read past end of buffer");
  }

  T data = *reinterpret_cast<const T*>(&data_[offset]);

  if (doSwapEndian)
    swapEndian<T>(data);

  return data;
}

template<> ByteBuffer& ByteBuffer::write<std::string>(std::string data);
template<> const std::string ByteBuffer::read<std::string>(bool doSwapEndian, bool null_terminated_string);
template<> ByteBuffer& ByteBuffer::write<std::wstring>(std::wstring data);
template<> const std::wstring ByteBuffer::read<std::wstring>(bool doSwapEndian, bool null_terminated_string);

}  // namespace swganh

template<typename T>
swganh::ByteBuffer& operator<<(swganh::ByteBuffer& buffer, const T& value) {
  buffer.write<T>(value);
  return buffer;
}
