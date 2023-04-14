#include "arbitrary_ntt.h"
#include "mod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>

TEST_CASE("arbitrary_ntt") {
  using Mod = ModT<1'000'000'007>;

  ArbitraryNTT<Mod> multiply;

  std::minstd_rand gen{Catch::getSeed()};
  std::uniform_int_distribution<> dist{0, Mod::mod() - 1};

  static constexpr int N = 50;

  auto sum_deg = GENERATE(range(0, N));
  auto f_deg = GENERATE(range(0, N));
  if (f_deg <= sum_deg) {
    auto g_deg = sum_deg - f_deg;
    std::vector<Mod> f(f_deg + 1), g(g_deg + 1);
    for (int i = 0; i <= f_deg; i++) {
      f[i] = Mod{dist(gen)};
    }
    for (int i = 0; i <= g_deg; i++) {
      g[i] = Mod{dist(gen)};
    }
    std::vector<Mod> expected_prod_fg(sum_deg + 1);
    for (int i = 0; i <= f_deg; ++i) {
      for (int j = 0; j <= g_deg; ++j) {
        expected_prod_fg[i + j] += f[i] * g[j];
      }
    }
    auto prod_fg = multiply(f, g);
    REQUIRE(prod_fg == expected_prod_fg);
  }
}
