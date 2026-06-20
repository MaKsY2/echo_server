#include "echo_server.hpp"

#include "tcp_connection.hpp"
#include "tcp_listener.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <system_error>

namespace epoll {
EchoServer::EchoServer(uint16_t port) : listener_(port) {
  std::cout << "listening on :" << port << std::endl;
}

void EchoServer::run() {
  while (!stop_.load()) {
    try {
      TcpConnection conn = listener_.accept();
      std::cout << "client connected (fd=" << conn.fd() << ")" << std::endl;
      handle(conn);
      std::cout << "client disconnected" << std::endl;
    } catch (const std::system_error &e) {
      if (stop_.load())
        break;
      throw;
    }
  }
}

void EchoServer::stop() noexcept { stop_.store(true); }

void EchoServer::handle(TcpConnection &conn) {
  std::array<std::byte, 4096> buf{};
  while (true) {
    size_t n = conn.read(buf);
    if (n == 0)
      return;
    conn.write_all({buf.data(), n});
  }
}

} // namespace epoll