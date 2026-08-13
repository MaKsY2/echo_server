#pragma once

#include "server.hpp"
#include "tcp_listener.hpp"
namespace echo {
class UringServer : public Server {
  TcpListener listener_;

public:
  explicit UringServer(size_t port);

  void stop();
  void run();
};
} // namespace echo