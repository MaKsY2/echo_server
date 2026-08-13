#pragma once

#include "server.hpp"
#include "tcp_listener.hpp"
#include <cstdint>
namespace echo {
class EpollServer : public Server {
  TcpListener listener_;

public:
  explicit EpollServer(uint16_t);

  void stop();
  void run();
};
} // namespace echo