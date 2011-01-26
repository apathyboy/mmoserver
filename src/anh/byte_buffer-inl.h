/**
 * SWG:ANH Sandbox
 *
 * @copyright   Copyright (c) 2010 The SWG:ANH Team
 * @author      Eric Barr <apathy@swganh.org>
 */

#ifndef ANH_BYTE_BUFFER_INL_H_
#define ANH_BYTE_BUFFER_INL_H_

#include <string>
#include <stdexcept>

#include "anh/utilities.h"

namespace anh {
    
template<typename T>
ByteBuffer& ByteBuffer::write(T data) {
  write(reinterpret_cast<unsigned char*>(&data), sizeof(T));
  return *this;
}

template<typename T>
ByteBuffer& ByteBuffer::writeAt(size_t offset, T data) {
  write(offset, reinterpret_cast<unsigned char*>(&data), sizeof(T));
  return *this;
}

template<typename T>
const T ByteBuffer::read(bool doSwapEndian) {
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
      data = anh::swap_endian<T>(data);

  return data;
}

template<> ByteBuffer& ByteBuffer::write<std::string>(std::string data);
template<> const std::string ByteBuffer::read<std::string>(bool doSwapEndian);
template<> ByteBuffer& ByteBuffer::write<std::wstring>(std::wstring data);
template<> const std::wstring ByteBuffer::read<std::wstring>(bool doSwapEndian);

}  // namespace anh

template<typename T>
anh::ByteBuffer& operator<<(anh::ByteBuffer& buffer, const T& value) {
  buffer.write<T>(value);
  return buffer;
}

#endif  // ANH_BYTE_BUFFER_INL_H_
