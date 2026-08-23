#pragma once

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <syncstream>
#include <vector>

namespace common {
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

  void report(std::string_view label,
              const std::filesystem::path &dump_path = {}) {
    if (size_ == 0) {
      std::cout << std::format("{}: no samples\n", label);
      return;
    }

    if (!dump_path.empty())
      report_raw(dump_path);

    std::sort(samples_.begin(), samples_.begin() + size_);
    auto pct = [&](double p) { return samples_[size_t(p * (size_ - 1))]; };

    std::osyncstream(std::cout)
        << std::format("{} ({} samples), ns:\n  p50    = {}\n  p99    = {}\n"
                       "  p99.9  = {}\n  p99.99 = {}\n  max    = {}\n",
                       label, size_, pct(0.50), pct(0.99), pct(0.999),
                       pct(0.9999), samples_[size_ - 1]);
  }

private:
  void report_raw(const std::filesystem::path &path) const {
    std::ofstream out(path);
    if (!out) {
      std::cerr << "cant open: " << path << '\n';
      return;
    }

    for (uint64_t i = 0; i < size_; ++i) {
      out << samples_[i] << '\n';
    }
  }
};

} // namespace common