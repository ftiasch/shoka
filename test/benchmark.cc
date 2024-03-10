#include <catch2/catch_all.hpp>

#include <random>

TEST_CASE("log_2") {
  constexpr int N = 200'000;
  constexpr int M = 200'000;

  BENCHMARK("ctz") {
    std::minstd_rand gen{Catch::getSeed()};
    int result = 0;
    for (int _ = 0; _ < M; _++) {
      auto l = std::uniform_int_distribution{1, N}(gen);
      result += 31 - __builtin_clz(l);
    }
    return result;
  };

  BENCHMARK_ADVANCED("tbl")(Catch::Benchmark::Chronometer meter) {
    std::vector<int> log(N + 1);
    log[1] = 0;
    for (int i = 2; i <= N; ++i) {
      log[i] = log[i - 1] + ((1 << (log[i - 1] + 1)) < i);
    }
    meter.measure([&log, N] {
      std::minstd_rand gen{Catch::getSeed()};
      int result = 0;
      for (int _ = 0; _ < M; _++) {
        auto l = std::uniform_int_distribution{1, N}(gen);
        result += log[l];
      }
      return result;
    });
  };
}
