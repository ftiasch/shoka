#include "barrett.h"
#include "binpow.h"
#include "mod.h"
#include "montgomery.h"
#include "non_const_mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

constexpr uint32_t MOD_32 = 998'244'353;
// https://primes.utm.edu/lists/2small/0bit.html
constexpr uint64_t MOD_64 = (1ULL << 62) - 57;

TEMPLATE_TEST_CASE("mod_32", "[template]", ModT<MOD_32>, MontgomeryT<MOD_32>,
                   BarrettModT<>, NonConstModT<>) {
  using Mod = TestType;

  Mod::set_mod(MOD_32);

  static constexpr auto N = 1'000;

  SECTION("constructor") {
    REQUIRE(Mod{233}.get() == 233);
    for (int i = 0; i < N; ++i) {
      REQUIRE(Mod{i}.get() == i);
    }
  }

  SECTION("normalization") {
    for (int i = 0; i < N; ++i) {
      REQUIRE(Mod::normalize(233 + i * static_cast<uint64_t>(MOD_32)).get() ==
              233);
    }
  }

  SECTION("negation") {
    REQUIRE((-Mod{0}).get() == 0);
    REQUIRE((-Mod{233}).get() == MOD_32 - 233);
  }

  SECTION("addition & subtraction") {
    Mod sum{0};
    for (int i = 0; i < N; ++i) {
      sum += Mod{1'000'000 * i};
    }
    for (int i = 0; i < N; ++i) {
      sum -= Mod{1'000'000 * i};
    }
    REQUIRE(sum.get() == 0);
  }

  SECTION("power") { REQUIRE(binpow(Mod{233}, MOD_32 - 1).get() == 1); }

  BENCHMARK("bench") {
    Mod result{1};
    for (int i = 1; i < N; ++i) {
      result *= Mod{i};
    }
    REQUIRE(result.get() == 815987315);
    return result;
  };
}

TEMPLATE_TEST_CASE("mod_64", "[template]", Mod64T<MOD_64>,
                   Montgomery64T<MOD_64>, BarrettMod64T<>, NonConstMod64T<>) {
  using Mod = TestType;
  Mod::set_mod(MOD_64);

  REQUIRE(Mod{233}.get() == 233);
  {
    int x = 233;
    REQUIRE(Mod{x}.get() == 233);
  }

  REQUIRE(binpow(Mod{233}, MOD_64 - 1).get() == 1);

  {
    Mod x{233};
    REQUIRE((x * x.inv()).get() == 1);
  }

  BENCHMARK("bench") {
    static constexpr auto N = 1'000;
    Mod result{1};
    for (int i = 1; i < N; ++i) {
      result *= Mod{i};
    }
    REQUIRE(result.get() == 4017367128501485088);
    return result;
  };
}
