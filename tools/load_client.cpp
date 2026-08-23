#include <arpa/inet.h>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <format>
#include <iostream>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>

#include "config.hpp"
#include "error.hpp"
#include "time_recorder.hpp"

void send_all(int fd, std::span<const std::byte> data) {
  size_t written = 0;
  while (written < data.size()) {
    ssize_t n =
        ::send(fd, data.data() + written, data.size() - written, MSG_NOSIGNAL);
    if (n < 0) {
      if (errno == EINTR)
        continue;
      common::throw_errno("send");
    }
    written += static_cast<size_t>(n);
  }
}

void recv_all(int fd, std::span<std::byte> data) {
  size_t received = 0;
  while (received < data.size()) {
    ssize_t n = ::recv(fd, data.data() + received, data.size() - received, 0);

    if (n < 0) {
      if (errno == EINTR)
        continue;
      common::throw_errno("receive");
    }
    if (n == 0) {
      throw std::runtime_error("server closed connection mid-run");
    }
    received += static_cast<size_t>(n);
  }
}

int main(int argc, char **argv) {
  try {

    common::Config cfg = common::parse_args(argc, argv);

    int fd = ::socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) {
      common::throw_errno("socket");
    }
    sockaddr_in addr{
        .sin_family = AF_INET,
        .sin_port = htons(cfg.port),
        .sin_addr{},
        .sin_zero{},
    };

    if (inet_pton(AF_INET, cfg.host, &addr.sin_addr) != 1) {
      throw std::runtime_error("bad host");
    }
    if (::connect(fd, (sockaddr *)&addr, sizeof(addr)) < 0) {
      common::throw_errno("connect");
    }

    int one = 1;
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &one, sizeof(one)) < 0) {
      common::throw_errno("setsockopt(TCP_NODELAY)");
    }

    const uint64_t total = cfg.rate * cfg.duration_s;
    const uint64_t warmup = cfg.rate * cfg.warmup_s;
    const uint64_t interval = 1'000'000'000ull / cfg.rate;

    std::vector<std::byte> tx(cfg.msg_size, std::byte{0xAB});
    std::vector<std::byte> rx(cfg.msg_size);
    const uint64_t cap = total - warmup;

    const std::string run_id = common::make_run_id();
    const std::filesystem::path raw_dir = "results/raw";
    std::filesystem::create_directories(raw_dir);
    auto dump = [&](std::string_view metric) {
      return raw_dir / std::format("{}.{}.txt", run_id, metric);
    };

    common::LazyRecorder rec(cap, dump("total"));
    common::LazyRecorder sched(cap, dump("sched_lag"));
    common::LazyRecorder send_rec(cap, dump("send_syscall"));
    common::LazyRecorder wait_rec(cap, dump("wait_for_echo"));

    std::cout << std::format(
        "run_id={} host={} port={} rate={} duration={}s msg={} warmup={}s\n",
        run_id, cfg.host, cfg.port, cfg.rate, cfg.duration_s, cfg.msg_size,
        cfg.warmup_s);

    const uint64_t start = common::now_ns() + 1'000'000;

    for (uint64_t i = 0; i < total; ++i) {
      const uint64_t t_next = start + i * interval;

      while (common::now_ns() < t_next) {
      }

      const uint64_t t0 = common::now_ns();
      std::memcpy(tx.data(), &t0, sizeof(t0));
      send_all(fd, tx);
      const uint64_t t1 = common::now_ns();
      recv_all(fd, rx);
      const uint64_t t2 = common::now_ns();

      if (i >= warmup) {
        rec.record(t2 - t_next);
        sched.record(t0 - t_next);
        send_rec.record(t1 - t0);
        wait_rec.record(t2 - t1);
      }
    }

    rec.report("total (from schedule)");
    sched.report("sched_lag");
    send_rec.report("send_syscall");
    wait_rec.report("wait_for_echo");

    close(fd);
    return 0;
  } catch (const std::exception &err) {
    fprintf(stderr, "fatal: %s\n", err.what());
    return 1;
  }
}