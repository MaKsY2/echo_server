#include "echo_server.hpp"

#include "tcp_connection.hpp"
#include "tcp_listener.hpp"

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <syncstream>
#include <system_error>
#include <thread>

namespace epoll {
EchoServer::EchoServer(uint16_t port) : listener_(port) {
  std::cout << "listening on :" << port << std::endl;
}

void EchoServer::run() {
  while (!stop_.load(std::memory_order_relaxed)) {
    try {
      TcpConnection conn = listener_.accept();
      {
        std::lock_guard lk(mu_);
        ++active_;
      }
      std::thread([this, c = std::move(conn)]() mutable {
        std::osyncstream(std::cout)
            << "client connected (fd=" << c.fd() << ")" << std::endl;
        try {
          handle(std::move(c));
        } catch (const std::exception &e) {
          std::osyncstream(std::cerr)
              << "clinet err0r: " << e.what() << std::endl;
        }
        std::osyncstream(std::cout) << "client disconnected" << std::endl;
        std::lock_guard lk(mu_);
        if (--active_ == 0)
          drained_.notify_all();
      }).detach();
    } catch (const std::system_error &e) {
      if (stop_.load())
        break;
      throw;
    }
  }
}

void EchoServer::stop() {
  stop_.store(true, std::memory_order_relaxed);
  listener_.shutdown();
  std::unique_lock lk(mu_);
  drained_.wait(lk, [this] { return active_ == 0; });
}

void EchoServer::handle(TcpConnection conn) {
  std::array<std::byte, 4096> buf{};
  while (true) {
    size_t n = conn.read(buf);
    if (n == 0)
      return;
    conn.write_all({buf.data(), n});
  }
}

} // namespace epoll