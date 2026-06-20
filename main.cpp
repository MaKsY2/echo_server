#include "echo_server.hpp"

#include <exception>
#include <iostream>

int main() {
  try {
    epoll::EchoServer server{8080};
    server.run();
  } catch (const std::exception &e) {
    std::cerr << "fatal: " << e.what() << std::endl;
    return 1;
  }
}