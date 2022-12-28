#include "barrett.h"
#include "binpow.h"
#include "mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("barrett") {
  SECTION("BarrettModT") {
    using Mod = BarrettModT<>;

    Mod::set_mod(998'244'353);

    REQUIRE(Mod{233}.get() == 233);
    {
      int x = 233;
      REQUIRE(Mod{x}.get() == 233);
    }
    // (998'244'353 << 8) + 233
    REQUIRE(Mod::normalize(255550554601).get() == 233);

    REQUIRE(binpow(Mod{233}, 998'244'352).get() == 1);

    {
      Mod x{233};
      REQUIRE((x * x.inv()).get() == 1);
    }
  }
  SECTION("BarrettMod64T") {
    // https://primes.utm.edu/lists/2small/0bit.html
    auto MOD = GENERATE(998'244'353ULL, (1ULL << 63) - 25);

    using Mod = BarrettMod64T<>;
    Mod::set_mod(MOD);

    REQUIRE(Mod{233}.get() == 233);
    {
      int x = 233;
      REQUIRE(Mod{x}.get() == 233);
    }

    REQUIRE(binpow(Mod{233}, MOD - 1).get() == 1);

    {
      Mod x{233};
      REQUIRE((x * x.inv()).get() == 1);
    }
  }
}

TEST_CASE("barrett_bench") {
  static constexpr auto N = 1'000;
  static constexpr auto MOD = (1ULL << 63) - 25;

  BENCHMARK("const_mod") {
    uint64_t result = 1;
    for (int i = 1; i < N; ++ i) {
      result = static_cast<__uint128_t>(result) * i % MOD;
    }
    REQUIRE(result == 7589003641238381426);
    return result;
  };

  BENCHMARK("barrett") {
    using Mod = BarrettMod64T<>;
    Mod::set_mod(MOD);

    Mod result{1};
    for (int i = 1; i < N; ++ i) {
      result *= Mod{i};
    }
    REQUIRE(result.get() == 7589003641238381426);
    return result;
  };
}
