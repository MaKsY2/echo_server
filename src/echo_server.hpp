#pragma once

#include "tcp_listener.hpp"
#include <atomic>
#include <cstdint>

namespace epoll {

class TcpConnection;

class EchoServer {
  TcpListener listener_;
  std::atomic<bool> stop_{false};

public:
  explicit EchoServer(uint16_t);
  void run();
  void stop() noexcept;

private:
  static void handle(TcpConnection &conn);
};
} // namespace epoll