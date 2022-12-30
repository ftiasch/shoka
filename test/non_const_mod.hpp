#include "barrett.h"
#include "binpow.h"
#include "non_const_mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("barrett_bench") {
  static constexpr auto N = 1'000;
  static constexpr auto MOD = (1ULL << 63) - 25;

  BENCHMARK("const_mod") {
    uint64_t result = 1;
    for (int i = 1; i < N; ++i) {
      result = static_cast<__uint128_t>(result) * i % MOD;
    }
    REQUIRE(result == 7589003641238381426);
    return result;
  };

  BENCHMARK("barrett") {
    using Mod = BarrettMod64T<>;
    Mod::set_mod(MOD);

    Mod result{1};
    for (int i = 1; i < N; ++i) {
      result *= Mod{i};
    }
    REQUIRE(result.get() == 7589003641238381426);
    return result;
  };
}