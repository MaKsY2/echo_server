#pragma once

#include "file_descriptor.hpp"

#include <cstddef>
#include <span>

namespace epoll {

class TcpConnection {
  FileDescriptor fd_;

public:
  explicit TcpConnection(FileDescriptor fd);
  [[nodiscard]] size_t read(std::span<std::byte>);
  [[nodiscard]] int fd() const noexcept;
  void write_all(std::span<const std::byte> data);
};
} // namespace epoll
