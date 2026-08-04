#pragma once
#include "file_descriptor.hpp"

#include <cstdint>

namespace echo {
class TcpConnection;

class TcpListener {
  FileDescriptor fd_;

public:
  explicit TcpListener(uint16_t port, int backlog = 128);
  TcpConnection accept();
  void shutdown() noexcept;
};
} // namespace echo
