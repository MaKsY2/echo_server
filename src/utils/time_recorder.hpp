#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <vector>

namespace utils {
inline uint64_t now_ns() {
  timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return uint64_t(ts.tv_sec) * 1'000'000'000ull + ts.tv_nsec;
}

class LazyRecorder {
  std::vector<uint64_t> samples_;
  uint64_t size_;

public:
  explicit LazyRecorder(uint64_t buffer) : size_(0) {
    samples_.resize(buffer);
    std::fill(samples_.begin(), samples_.end(), 0);
  }

  void record(uint64_t ns) noexcept {

    if (size_ < samples_.size()) {
      samples_[size_++] = ns;
    }

    // пох ну эксепт и эксепт, че падать то...
  }

  void report(const char *label) {
    if (size_ == 0) {
      printf("%s: no samples\n", label);
      return;
    }
    std::sort(samples_.begin(), samples_.begin() + size_);
    auto pct = [&](double p) { return samples_[size_t(p * (size_ - 1))]; };
    printf("%s (%zu samples), ns:\n", label, size_);
    printf("  p50    = %lu\n", pct(0.50));
    printf("  p99    = %lu\n", pct(0.99));
    printf("  p99.9  = %lu\n", pct(0.999));
    printf("  max    = %lu\n", samples_[size_ - 1]);
  }
};

} // namespace utils