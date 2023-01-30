#include "mod.h"
#include "poly_gen_dyn.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators.hpp>

TEST_CASE("poly_gen_dyn") {
  using Mod = ModT<998'244'353>;
  using PolyGen = PolyGenDynT<Mod>;
  using Vector = std::vector<Mod>;

  auto take = [&](auto w, int n) {
    std::vector<Mod> p(n);
    for (int i = 0; i < n; ++i) {
      p[i] = w[i];
    }
    return p;
  };

  SECTION("geo_sum_1") {
    // f(z) = f(z) * z + 1
    auto [f, uf] = PolyGen::var();
    auto rhs = f.shift(1) + PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    REQUIRE(take(f, 2) == Vector{Mod{1}, Mod{1}});
  }

  SECTION("geo_sum_2") {
    // f(z) = f(z) * z + 1
    auto [f, uf] = PolyGen::var();
    auto rhs = f * PolyGen::value({Mod{0}, Mod{1}}) + PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    REQUIRE(take(f, 2) == Vector{Mod{1}, Mod{1}});
  }

  SECTION("geo_sum_short_zealous") {
    // f(z) = f(z) * (z + z^2 + z^3) + 1
    auto [f, uf] = PolyGen::var();
    auto rhs = f * PolyGen::value({Mod{0}, Mod{1}, Mod{1}, Mod{1}}) +
               PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    REQUIRE(take(f, 5) == Vector{Mod{1}, Mod{1}, Mod{2}, Mod{4}, Mod{7}});
  }

  SECTION("fib") {
    // f(z) = f(z) * (z + z^2) + 1
    auto [f, uf] = PolyGen::var();
    auto rhs =
        f * PolyGen::value({Mod{0}, Mod{1}, Mod{1}}) + PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    REQUIRE(take(f, 5) == Vector{Mod{1}, Mod{1}, Mod{2}, Mod{3}, Mod{5}});
    REQUIRE(f[100000] == Mod{56136314});
  }

  SECTION("exp") {
    // f(z) = exp(z)
    // <=> f(z) = (\int f(z)) + 1
    auto [f, uf] = PolyGen::var();
    auto rhs = f.integrate() + PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    REQUIRE(f[0] == Mod{1});
    REQUIRE(f[1] == Mod{1});
    REQUIRE(f[2] * Mod{2} == Mod{1});
    REQUIRE(f[3] * Mod{6} == Mod{1});
    REQUIRE(f[4] * Mod{24} == Mod{1});
  }

  auto check_catalan = [&](auto f) {
    REQUIRE(take(f, 10) == Vector{Mod{1}, Mod{1}, Mod{2}, Mod{5}, Mod{14},
                                  Mod{42}, Mod{132}, Mod{429}, Mod{1430},
                                  Mod{4862}});
  };

  SECTION("catalan_1") {
    // f(z) = f(z) * f(z) * z + 1
    auto [f, uf] = PolyGen::var();
    auto rhs =
        (f * f) * PolyGen::value({Mod{0}, Mod{1}}) + PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    check_catalan(f);
  }

  SECTION("catalan_2") {
    // f(z) = f(z) * f(z) * z + 1
    auto [f, uf] = PolyGen::var();
    auto rhs =
        f * (f * PolyGen::value({Mod{0}, Mod{1}})) + PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    check_catalan(f);
  }

  SECTION("catalan_3") {
    // f(z) = f(z) * f(z) * z + 1
    auto [f, uf] = PolyGen::var();
    auto rhs =
        f * (PolyGen::value({Mod{0}, Mod{1}}) * f) + PolyGen::value({Mod{1}});
    uf->delegate(rhs);
    check_catalan(f);
  }
}
