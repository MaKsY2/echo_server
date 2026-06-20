#include "tcp_listener.hpp"
#include "error.hpp"
#include "tcp_connection.hpp"

#include <arpa/inet.h>
#include <cstdint>
#include <cstring>
#include <netinet/in.h>
#include <sys/socket.h>

namespace epoll {
TcpListener::TcpListener(uint16_t port, int backlog) {
  FileDescriptor sock{::socket(AF_INET, SOCK_STREAM, 0)};
  if (!sock.valid())
    throw_errno("socket");

  int opt = 1;

  if (::setsockopt(sock.get(), SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) <
      0) {
    throw_errno("setsockopt(SO_REUSEADDR)");
  }

  sockaddr_in addr{};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = INADDR_ANY;

  if (::bind(sock.get(), reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) <
      0) {
    throw_errno("bind");
  }
  if (::listen(sock.get(), backlog) < 0) {
    throw_errno("listen");
  }
  fd_ = std::move(sock);
}

void TcpListener::shutdown() noexcept { ::shutdown(fd_.get(), SHUT_RDWR); }

TcpConnection TcpListener::accept() {
  while (true) {
    int client = ::accept(fd_.get(), nullptr, nullptr);
    if (client >= 0)
      return TcpConnection{FileDescriptor{client}};
    if (errno == EINTR)
      continue;
    throw_errno("accept");
  }
}
} // namespace epoll