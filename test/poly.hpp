#include "mod.h"
#include "mont.h"
#include "poly.h"
#include "poly_div.h"
#include "poly_exp.h"
#include "poly_inv.h"
#include "poly_log.h"
#include "poly_multieval.h"
#include "poly_prod.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace poly {

constexpr uint32_t MOD = 998'244'353;

template <typename Mod> struct RandomPoly {
public:
  using Poly = PolyT<Mod>;

  explicit RandomPoly()
      : gen{Catch::getSeed()}, dist0{0, Mod::mod() - 1},
        dist1{1, Mod::mod() - 1} {}

  Poly operator()(int deg, bool invertible = false) {
    Poly r(deg + 1);
    for (int i = 0; i <= deg; ++i) {
      r[i] = mod0();
    }
    if (invertible) {
      r[0] = mod1();
    }
    return r;
  }

private:
  Mod mod0() { return Mod{dist0(gen)}; }
  Mod mod1() { return Mod{dist1(gen)}; }

  std::minstd_rand gen;
  std::uniform_int_distribution<> dist0, dist1;
};

template <typename Mod> Mod slow_eval(const std::vector<Mod> &c, Mod x) {
  Mod result{0};
  for (int i = c.size(); i--;) {
    result = result * x + c[i];
  }
  return result;
}

} // namespace poly

TEMPLATE_TEST_CASE("poly", "[template]", ModT<poly::MOD>, MontT<poly::MOD>) {
  using namespace poly;

  using Mod = TestType;
  using Poly = PolyT<Mod>;

  RandomPoly<Mod> random_poly;

  SECTION("constructor") {
    REQUIRE(Poly{Mod{1}, Mod{2}}.deg() == 1);
    std::vector<Mod> v{Mod{1}, Mod{2}};
    REQUIRE(Poly(v).deg() == 1);
    REQUIRE(Poly(3).deg() == 2);
  }

  static constexpr int N = 32;

  SECTION("addition") {
    auto f_deg = GENERATE(range(-1, N));
    auto g_deg = GENERATE(range(-1, N));
    auto f = random_poly(f_deg);
    auto g = random_poly(g_deg);
    auto sum_fg = f + g;
    Poly expected_sum_fg(std::max(f_deg, g_deg) + 1);
    for (int i = 0; i <= std::max(f_deg, g_deg); ++i) {
      if (i <= f_deg) {
        expected_sum_fg[i] += f[i];
      }
      if (i <= g_deg) {
        expected_sum_fg[i] += g[i];
      }
    }
    REQUIRE(sum_fg == expected_sum_fg);
  }

  SECTION("subtracion") {
    auto sum_deg = GENERATE(range(0, N));
    auto f_deg = GENERATE(range(0, N));
    if (f_deg <= sum_deg) {
      auto g_deg = sum_deg - f_deg;
      auto f = random_poly(f_deg);
      auto g = random_poly(g_deg);
      Poly neg_g(g_deg + 1);
      for (int i = 0; i <= g_deg; ++i) {
        neg_g[i] = -g[i];
      }
      auto diff_fg = f - g;
      auto alt_diff_fg = f + neg_g;
      REQUIRE(diff_fg == alt_diff_fg);
    }
  }

  SECTION("multiplication") {
    auto sum_deg = GENERATE(range(0, N));
    auto f_deg = GENERATE(range(0, N));
    if (f_deg <= sum_deg) {
      auto g_deg = sum_deg - f_deg;
      auto f = random_poly(f_deg);
      auto g = random_poly(g_deg);
      Poly expected_prod_fg(sum_deg + 1);
      for (int i = 0; i <= f_deg; ++i) {
        for (int j = 0; j <= g_deg; ++j) {
          expected_prod_fg[i + j] += f[i] * g[j];
        }
      }
      auto prod_fg = f * g;
      REQUIRE(prod_fg == expected_prod_fg);
      f *= g;
      REQUIRE(f == expected_prod_fg);
    }
  }

  SECTION("inverse") {
    auto f_deg = GENERATE(range(0, N));
    auto f = random_poly(f_deg, true);
    PolyInv<Poly> inv{};
    auto inv_f = inv(f);
    REQUIRE(inv_f.deg() == f.deg());
    auto id = f * inv_f;
    id.resize(f_deg + 1);
    REQUIRE(id[0] == Mod{1});
    for (int i = 1; i <= f_deg; ++i) {
      REQUIRE(id[i] == Mod{0});
    }
  }

  SECTION("division") {
    auto f_deg = GENERATE(range(0, N));
    auto g_deg = GENERATE(range(0, N));
    auto f = random_poly(f_deg);
    auto g = random_poly(g_deg, true);
    PolyDiv<Poly> div{};
    auto q = div(f, g);
    REQUIRE(q.deg() == std::max(f.deg(), g.deg()));
    auto gq = g * q;
    gq.resize(f.size());
    REQUIRE(f == gq);
  }

  SECTION("logarithm") {
    auto f_deg = GENERATE(range(0, N));
    auto f = random_poly(f_deg);
    f[0] = Mod{1};
    PolyLog<Poly> log{};
    auto log_f = log(f);
    REQUIRE(log_f.deg() == f_deg);
    Poly expected_log_f(f_deg + 1);
    for (int i = 1; i <= f_deg; ++i) {
      expected_log_f[i] = Mod{i} * f[i];
      for (int j = 1; j <= i; ++j) {
        expected_log_f[i] -= f[j] * expected_log_f[i - j];
      }
    }
    for (int i = 1; i <= f_deg; ++i) {
      expected_log_f[i] *= Mod{i}.inv();
    }
    REQUIRE(log_f == expected_log_f);
  }

  SECTION("exponentiation") {
    auto f_deg = GENERATE(range(0, N));
    auto f = random_poly(f_deg);
    f[0] = Mod{0};
    PolyExp<Poly> exp{};
    auto exp_f = exp(f);
    REQUIRE(exp_f.deg() == f_deg);
    std::vector<Mod> expected_exp_f(f_deg + 1), g(f_deg + 1);
    for (int i = 1; i <= f_deg; ++i) {
      g[i] = Mod{i} * f[i];
    }
    expected_exp_f[0] = Mod{1};
    for (int i = 1; i <= f_deg; ++i) {
      for (int j = 0; j < i; ++j) {
        expected_exp_f[i] += g[j + 1] * expected_exp_f[i - 1 - j];
      }
      expected_exp_f[i] *= Mod{i}.inv();
    }
    REQUIRE(exp_f == expected_exp_f);
  }

  SECTION("multi_eval") {
    auto f_deg = GENERATE(range(0, N));
    auto f = random_poly(f_deg).vector();
    auto m = GENERATE(0, N);
    auto a = random_poly(m - 1).vector();
    PolyMultiEval<Poly> eval;
    auto result = eval(f, a);
    for (int i = 0; i < m; ++i) {
      REQUIRE(result[i].get() == slow_eval(f, a[i]).get());
    }
  }

  SECTION("multi_prod") {
    auto n = GENERATE(range(0, N));
    std::vector<Poly> mons(n);
    for (int i = 0; i < n; ++i) {
      mons[i] = random_poly(3);
    }
    PolyProduct<Poly> prod;
    auto p = prod(mons);
    Poly expected_p{Mod{1}};
    for (auto &&mon : mons) {
      expected_p *= mon;
    }
    REQUIRE(p == expected_p);
  }
}
