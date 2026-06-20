#pragma once

#include "tcp_listener.hpp"
#include <cstdint>

namespace epoll {

class TcpConnection;

class EchoServer {
  TcpListener listener_;

public:
  explicit EchoServer(uint16_t);
  void run();

private:
  static void handle(TcpConnection &conn);
};
} // namespace epoll