#pragma once

#include "tcp_listener.hpp"
#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>

namespace epoll {

class TcpConnection;

class EchoServer {
  TcpListener listener_;
  std::atomic<bool> stop_{false};
  std::mutex mu_;
  std::condition_variable drained_;
  std::size_t active_ = 0;

public:
  explicit EchoServer(uint16_t);
  void run();
  void stop();

private:
  static void handle(TcpConnection conn);
};
} // namespace epoll