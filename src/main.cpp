#include "epoll_server.hpp"
#include "parsing.hpp"
#include "server.hpp"
#include "threaded_server.hpp"
#include "uring_server.hpp"

#include <exception>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>

int main() {
  try {
    std::unique_ptr<echo::Server> server;

    std::string server_cfg;

    std::getline(std::cin, server_cfg);

    const auto server_type = common::ParseServerType(server_cfg);
    switch (server_type) {
    case common::ServerType::Epoll:
      server = std::make_unique<echo::EpollServer>(8080);
    case common::ServerType::Uring:
      server = std::make_unique<echo::UringServer>(8080);
    case common::ServerType::Threaded:
      server = std::make_unique<echo::ThreadedServer>(8080);
      break;
    case common::ServerType::Unknown:
      throw std::invalid_argument("invalid Server Type");
      break;
    }

    server->run();
  } catch (const std::exception &e) {
    std::cerr << "fatal: " << e.what() << std::endl;
    return 1;
  }
}