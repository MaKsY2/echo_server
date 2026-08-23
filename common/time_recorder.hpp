#pragma once

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <string>
#include <string_view>
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
  std::filesystem::path dump_path_;

public:
  explicit LazyRecorder(uint64_t buffer, const std::filesystem::path &dump_path)
      : size_(0), dump_path_(dump_path) {
    samples_.resize(buffer);
    std::fill(samples_.begin(), samples_.end(), 0);
  }

  void record(uint64_t ns) noexcept {

    if (size_ < samples_.size()) {
      samples_[size_++] = ns;
    }
  }

  void report(std::string_view label) {
    if (size_ == 0) {
      std::cout << std::format("{}: no samples\n", label);
      return;
    }

    report_raw();

    std::sort(samples_.begin(), samples_.begin() + size_);
    auto pct = [&](double p) { return samples_[size_t(p * (size_ - 1))]; };

    std::osyncstream(std::cout)
        << std::format("{} ({} samples), ns:\n  p50    = {}\n  p99    = {}\n"
                       "  p99.9  = {}\n  p99.99 = {}\n  max    = {}\n",
                       label, size_, pct(0.50), pct(0.99), pct(0.999),
                       pct(0.9999), samples_[size_ - 1]);
  }

private:
  void report_raw() const {
    std::ofstream out(dump_path_);
    if (!out) {
      std::cerr << "cant open: " << dump_path_ << '\n';
      return;
    }

    for (uint64_t i = 0; i < size_; ++i) {
      out << samples_[i] << '\n';
    }
  }
};

inline std::string make_run_id() {
  using namespace std::chrono;
  const zoned_time zt{current_zone(), floor<seconds>(system_clock::now())};
  return std::format("{:%Y%m%d_%H%M%S}", zt);
}

} // namespace common