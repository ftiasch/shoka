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

template <typename Mod> constexpr Mod slow_gauss(Mod a) {
  const auto MOD = Mod::mod();
  auto x = a.get();
  Mod inv{1};
  while (x > 1) {
    inv *= Mod{MOD - MOD / x};
    x = MOD % x;
  }
  return inv;
}

template <typename Mod> constexpr Mod gauss(Mod a) {
  const auto MOD = Mod::mod();
  auto t = a.get();
  Mod inv{1};
  while (t > 1) {
    auto r = MOD % t;
    auto q = MOD / t;
    if (r + r <= t) {
      inv *= Mod{MOD - q};
      t = r;
    } else {
      inv *= Mod{q + 1};
      t = t - r;
    }
  }
  return inv;
}

} // namespace mod

TEST_CASE("mod_inv") {
  constexpr uint32_t SMALL_MOD = (1 << 18) - 5;

  BENCHMARK("gauss") {
    using Mod = ModT<SMALL_MOD>;

    Mod result{1};
    for (int i = 1; i < SMALL_MOD; ++i) {
      result *= mod::slow_gauss(Mod{i});
    }
    REQUIRE(result == -Mod{1});
  };

  BENCHMARK("gauss_faster") {
    using Mod = ModT<SMALL_MOD>;

    Mod result{1};
    for (int i = 1; i < SMALL_MOD; ++i) {
      result *= mod::gauss(Mod{i});
    }
    REQUIRE(result == -Mod{1});
  };

  BENCHMARK("fermat") {
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
