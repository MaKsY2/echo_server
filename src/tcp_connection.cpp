#include "tcp_connection.hpp"
#include "error.hpp"

#include <cerrno>
#include <sys/socket.h>
#include <utility>

namespace epoll {
TcpConnection::TcpConnection(FileDescriptor fd) : fd_(std::move(fd)) {}

[[nodiscard]] size_t TcpConnection::read(std::span<std::byte> buf) {
  while (true) {
    ssize_t n = ::read(fd_.get(), buf.data(), buf.size());
    if (n >= 0)
      return static_cast<size_t>(n);
    if (errno == EINTR)
      continue;
    throw_errno("read");
  }
}

void TcpConnection::write_all(std::span<const std::byte> data) {
  size_t written = 0;
  while (written < data.size()) {
    ssize_t n = ::send(fd_.get(), data.data() + written, data.size() - written,
                       MSG_NOSIGNAL);
    if (n < 0) {
      if (errno == EINTR)
        continue;
      throw_errno("send");
    }
    written += static_cast<size_t>(n);
  }
}

[[nodiscard]] int TcpConnection::fd() const noexcept { return fd_.get(); }
} // namespace epoll