#include "tcp_connection.hpp"
#include "error.hpp"

#include <utility>

namespace epoll {
TcpConnection::TcpConnection(FileDescriptor fd) : fd_(std::move(fd)) {}

[[nodiscard]] size_t TcpConnection::read(std::span<std::byte> buf) {
  ssize_t n = ::read(fd_.get(), buf.data(), buf.size());
  if (n < 0)
    throw_errno("read");
  return static_cast<size_t>(n);
}

void TcpConnection::write_all(std::span<const std::byte> data) {
  size_t written = 0;
  while (written < data.size()) {
    ssize_t n =
        ::write(fd_.get(), data.data() + written, data.size() - written);
    if (n < 0)
      throw_errno("write");
    written += static_cast<size_t>(n);
  }
}

[[nodiscard]] int TcpConnection::fd() const noexcept { return fd_.get(); }
} // namespace epoll