#include "mod.h"
#include "polynomial_interpolation.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators_all.hpp>

TEST_CASE("polynomial_interpolation") {
  constexpr int d = 100;
  using Mod = mod::ModT<998'244'353>;

  auto eval = [&](const std::vector<Mod> &coef, Mod x) {
    Mod result{0};
    for (int i = coef.size(); i--;) {
      result = result * x + coef[i];
    }
    return result;
  };

  using namespace Catch::Generators;
  auto C = GENERATE(take(1, chunk(d, random(0U, Mod::MOD - 1))));
  std::vector<Mod> coef(d), values(d);
  for (int i = 0; i < d; ++i) {
    coef[i] = Mod(C[i]);
  }
  for (int i = 0; i < d; ++i) {
    values[i] = eval(coef, Mod(i));
  }

  auto n = GENERATE(take(d, random(0U, Mod::MOD - 1)));
  PolynomialInterpolation<Mod> interpolate(d);
  auto output = interpolate(values, Mod(n));
  auto answer = eval(coef, Mod(n));
  REQUIRE(output.get() == answer.get());
}
