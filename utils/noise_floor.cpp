#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <ctime>
#include <vector>

static inline uint64_t now_ns() {
    timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return uint64_t(ts.tv_sec) * 1'000'000'000ull + ts.tv_nsec;
}

int main() {
    constexpr size_t N = 10'000'000;
    std::vector<uint64_t> samples(N);  

    for (size_t i = 0; i < 1'000'000; ++i)
        samples[i % N] = now_ns();

    for (size_t i = 0; i < N; ++i) {
        uint64_t a = now_ns();
        uint64_t b = now_ns();
        samples[i] = b - a;
    }


    std::sort(samples.begin(), samples.end());
    auto pct = [&](double p) { return samples[size_t(p * (N - 1))]; };
    printf("noise floor, ns:\n");
    printf("  p50    = %lu\n", pct(0.50));
    printf("  p99    = %lu\n", pct(0.99));
    printf("  p99.9  = %lu\n", pct(0.999));
    printf("  p99.99 = %lu\n", pct(0.9999));
    printf("  max    = %lu\n", samples[N - 1]);
    return 0;
}