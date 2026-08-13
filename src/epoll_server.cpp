#include "epoll_server.hpp"
#include "tcp_listener.hpp"
#include <cstdint>

namespace echo {
EpollServer::EpollServer(uint16_t port) : listener_(port) {}

void EpollServer::stop() {}
void EpollServer::run() {}
} // namespace echo