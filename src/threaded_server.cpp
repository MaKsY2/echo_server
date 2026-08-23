#include "threaded_server.hpp"

#include "tcp_connection.hpp"
#include "tcp_listener.hpp"
#include "time_recorder.hpp"

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <mutex>
#include <syncstream>
#include <system_error>
#include <thread>

namespace echo {
ThreadedServer::ThreadedServer(uint16_t port) : listener_(port) {
  std::cout << "listening on :" << port << std::endl;
}

void ThreadedServer::run() {
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

void ThreadedServer::stop() {
  stop_.store(true, std::memory_order_relaxed);
  listener_.shutdown();
  std::unique_lock lk(mu_);
  drained_.wait(lk, [this] { return active_ == 0; });
}

void ThreadedServer::handle(TcpConnection conn) {
  common::LazyRecorder rec_inbound(1000000, "inbound_server");
  common::LazyRecorder rec_service(1000000, "service_server");
  std::array<std::byte, 4096> buf{};

  while (true) {
    size_t n = conn.read(buf);
    if (n == 0)
      break;
    uint64_t t_read = common::now_ns();
    if (n >= sizeof(uint64_t)) {
      uint64_t t0;
      std::memcpy(&t0, buf.data(), sizeof(t0));

      if (t0 < t_read && t_read - t0 < 2'000'000ULL) {
        rec_inbound.record(t_read - t0);
      }
    }
    conn.write_all({buf.data(), n});
    rec_service.record(common::now_ns() - t_read);
  }
  rec_inbound.report("server inbound (t0 -> read done)");
  rec_service.report("server service (read done -> write done)");
}

} // namespace echo