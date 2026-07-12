#pragma once

#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <string_view>

namespace common {

struct Config {
  const char *host = "127.0.0.1";
  uint16_t port = 8080;
  uint64_t rate = 100000;
  uint64_t duration_s = 10;
  std::size_t msg_size = 64;
  uint64_t warmup_s = 1;
};

inline Config parse_args(int argc, char **argv) {
  Config cfg;
  for (int i = 1; i < argc; ++i) {
    std::string_view a = argv[i];
    auto next = [&]() -> const char * {
      if (++i >= argc) {
        fprintf(stderr, "missing value for %s\n", a.data());
        exit(1);
      }
      return argv[i];
    };
    if (a == "--host")
      cfg.host = next();
    else if (a == "--port")
      cfg.port = atoi(next());
    else if (a == "--rate")
      cfg.rate = atoll(next());
    else if (a == "--duration")
      cfg.duration_s = atoll(next());
    else if (a == "--msg-size")
      cfg.msg_size = atoll(next());
    else if (a == "--warmup")
      cfg.warmup_s = atoll(next());
    else {
      fprintf(stderr, "unknown arg: %s\n", a.data());
      exit(1);
    }
  }
  return cfg;
}
} // namespace common