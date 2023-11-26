#include "mod.h"
#include "poly_interpolate.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators_all.hpp>

TEST_CASE("poly_interpolate") {
  static constexpr int d = 50;
  static constexpr uint32_t MOD = 998'244'353;
  using Mod = ModT<MOD>;

  auto eval = [&](const std::vector<Mod> &coef, Mod x) {
    Mod result{0};
    for (int i = coef.size(); i--;) {
      result = result * x + coef[i];
    }
    return result;
  };
  using namespace Catch::Generators;
  auto C = GENERATE(take(1, chunk(d, random(0U, MOD - 1))));
  std::vector<Mod> coef(d), values(d);
  for (int i = 0; i < d; ++i) {
    coef[i] = Mod{C[i]};
  }
  for (int i = 0; i < d; ++i) {
    values[i] = eval(coef, Mod{i});
  }
  auto n = GENERATE(take(d, random(0, 100)));
  auto result = poly_interpolate(values, Mod{n});
  auto expected = eval(coef, Mod{n});
  REQUIRE(result == expected);
}
