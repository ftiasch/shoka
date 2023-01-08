#include "barrett.h"
#include "binpow.h"
#include "dyn_mod.h"
#include "mod.h"
#include "mont.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

namespace mod {

static constexpr auto N = 1'000;

constexpr uint32_t MOD_32 = 998'244'353;
// https://primes.utm.edu/lists/2small/0bit.html
constexpr uint64_t MOD_64 = (1ULL << 62) - 57;

template <uint32_t MOD> constexpr uint32_t wrong_inv(uint32_t a) {
  return a == 1 ? 1
                : static_cast<uint64_t>(MOD - MOD / a) *
                      wrong_inv<MOD>(MOD % a) % MOD;
}

} // namespace mod

TEST_CASE("mod_inv") {
  constexpr uint32_t SMALL_MOD = (1 << 18) - 5;

  BENCHMARK("baseline") {
    uint32_t result{1};
    for (int i = 1; i < SMALL_MOD; ++i) {
      result = static_cast<uint64_t>(result) * mod::wrong_inv<SMALL_MOD>(i) %
               SMALL_MOD;
    }
    REQUIRE(result + 1 == SMALL_MOD);
  };

  BENCHMARK("bench") {
    using Mod = ModT<SMALL_MOD>;

    Mod result{1};
    for (int i = 1; i < SMALL_MOD; ++i) {
      result *= Mod{i}.inv();
    }
    REQUIRE(result == -Mod{1});
  };
}

TEMPLATE_TEST_CASE("mod_32", "[template]", ModT<mod::MOD_32>,
                   MontT<mod::MOD_32>, BarrettT<>, DynModT<>) {
  using namespace mod;

  using Mod = TestType;

  Mod::set_mod(MOD_32);

  { REQUIRE(Mod::mod() == MOD_32); }

  {
    for (int i = 0; i < N; ++i) {
      REQUIRE(Mod{i}.get() == i);
    }
  }

  {
    for (int i = 0; i < N; ++i) {
      REQUIRE(Mod::normalize(233 + i * static_cast<typename Mod::M2>(MOD_32)) ==
              Mod{233});
    }
  }

  {
    REQUIRE((-Mod{0}).get() == 0);
    REQUIRE((-Mod{233}).get() == MOD_32 - 233);
  }

  {
    Mod sum{0};
    for (int i = 0; i < N; ++i) {
      sum += Mod{1'000'000 * i};
    }
    for (int i = 0; i < N; ++i) {
      sum -= Mod{1'000'000 * i};
    }
    REQUIRE(sum == Mod{0});
  }

  { REQUIRE(binpow(Mod{233}, MOD_32 - 1) == Mod{1}); }

  BENCHMARK("bench") {
    Mod result{1};
    for (int i = 1; i < N; ++i) {
      result *= Mod{i};
    }
    REQUIRE(result.get() == 815987315);
    return result;
  };
}

TEMPLATE_TEST_CASE("mod_64", "[template]", Mod64T<mod::MOD_64>,
                   Mont64T<mod::MOD_64>, Barrett64T<>, DynMod64T<>) {
  using namespace mod;

  using Mod = TestType;
  Mod::set_mod(MOD_64);

  { REQUIRE(Mod::mod() == MOD_64); }

  {
    REQUIRE(Mod{233}.get() == 233);
    for (int i = 0; i < N; ++i) {
      REQUIRE(Mod{i}.get() == i);
    }
  }

  {
    for (int i = 0; i < N; ++i) {
      REQUIRE(Mod::normalize(233 + i * static_cast<typename Mod::M2>(MOD_64)) ==
              Mod{233});
    }
  }

  {
    REQUIRE((-Mod{0}).get() == 0);
    REQUIRE((-Mod{233}).get() == MOD_64 - 233);
  }

  {
    Mod sum{0};
    for (int i = 0; i < N; ++i) {
      sum += Mod{1'000'000 * i};
    }
    for (int i = 0; i < N; ++i) {
      sum -= Mod{1'000'000 * i};
    }
    REQUIRE(sum == Mod{0});
  }

  { REQUIRE(binpow(Mod{233}, MOD_64 - 1) == Mod{1}); }

  BENCHMARK("bench") {
    Mod result{1};
    for (int i = 1; i < N; ++i) {
      result *= Mod{i};
    }
    REQUIRE(result.get() == 4017367128501485088);
    return result;
  };
}
