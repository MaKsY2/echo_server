#pragma once

#include <cstdint>
#include <ctime>

namespace utils {
inline uint64_t now_ns() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return uint64_t(ts.tv_sec) * 1'000'000'000ull + ts.tv_nsec;
}

} // namespace utils