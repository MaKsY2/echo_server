#pragma once
#include "echo_server.hpp"
#include <gtest/gtest.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <atomic>
#include <string>
#include <thread>

using namespace std::chrono_literals;
using namespace epoll;

namespace tests {

class TestClient {
  int fd_{-1};

public:
  explicit TestClient(uint16_t port) {
    fd_ = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd_ < 0)
      throw std::runtime_error("socket");

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    if (::connect(fd_, reinterpret_cast<sockaddr *>(&addr), sizeof(addr)) < 0) {
      ::close(fd_);
      throw std::runtime_error("connect");
    }
  }

  ~TestClient() {
    if (fd_ >= 0)
      ::close(fd_);
  }
  TestClient(const TestClient &) = delete;
  TestClient &operator=(const TestClient &) = delete;

  void send(const std::string &s) {
    size_t sent = 0;
    while (sent < s.size()) {
      ssize_t n = ::write(fd_, s.data() + sent, s.size() - sent);
      ASSERT_GT(n, 0) << "write failed";
      sent += static_cast<size_t>(n);
    }
  }

  std::string recv_exact(size_t bytes) {
    std::string out;
    out.resize(bytes);
    size_t got = 0;
    while (got < bytes) {
      ssize_t n = ::read(fd_, out.data() + got, bytes - got);
      if (n <= 0)
        break;
      got += static_cast<size_t>(n);
    }
    out.resize(got);
    return out;
  }

  void close() {
    if (fd_ >= 0) {
      ::close(fd_);
      fd_ = -1;
    }
  }
};

uint16_t pick_port() {
  static std::atomic<uint16_t> next{18080};
  return next.fetch_add(1);
}

class ServerFixture : public ::testing::Test {
protected:
  uint16_t port_;
  std::unique_ptr<EchoServer> server_;
  std::thread thread_;

  void SetUp() override {
    port_ = pick_port();
    server_ = std::make_unique<EchoServer>(port_);
    thread_ = std::thread([this] {
      try {
        server_->run();
      } catch (...) {
      }
    });
    std::this_thread::sleep_for(50ms);
  }

  void TearDown() override {
    server_->stop();
    try {
      TestClient c(port_);
    } catch (...) {
    }
    if (thread_.joinable())
      thread_.join();
    server_.reset();
  }
};

} // namespace tests