#include "montgomery.h"
#include "poly.h"
#include "poly_div.h"
#include "poly_exp.h"
#include "poly_inv.h"
#include "poly_log.h"
#include "poly_multieval.h"

#include <bits/stdc++.h>

#include <catch2/catch_all.hpp>
#include <catch2/generators/catch_generators.hpp>

namespace poly {

using Mod = MontgomeryT<998'244'353>;
using Poly = PolyT<Mod>;

struct RandomPoly {
public:
  explicit RandomPoly()
      : gen{std::random_device{}()}, dist0{0, Mod::MOD - 1}, dist1{1, Mod::MOD -
                                                                          1} {}

  Poly get(int deg) {
    Poly r(deg + 1);
    for (int i = 0; i <= deg; ++i) {
      r[i] = mod0();
    }
    return r;
  }

  Poly get1(int deg) {
    Poly r(deg + 1);
    r[0] = mod1();
    for (int i = 1; i <= deg; ++i) {
      r[i] = mod0();
    }
    return r;
  }

private:
  Mod mod0() { return Mod{dist0(gen)}; }
  Mod mod1() { return Mod{dist1(gen)}; }

  std::minstd_rand gen;
  std::uniform_int_distribution<> dist0, dist1;
};

Mod slow_eval(const std::vector<Mod> &c, Mod x) {
  Mod result{0};
  for (int i = c.size(); i--;) {
    result = result * x + c[i];
  }
  return result;
}

} // namespace poly

TEST_CASE("poly") {
  using namespace poly;

  SECTION("constructor") {
    REQUIRE(Poly{Mod{1}, Mod{2}}.deg() == 1);
    REQUIRE(Poly{std::vector<Mod>{Mod{1}, Mod{2}}}.deg() == 1);
    REQUIRE(Poly(3).deg() == 2);
  }

  static constexpr int MAX_DEG = 32;

  SECTION("addition") {
    auto f_deg = GENERATE(range(-1, MAX_DEG));
    auto g_deg = GENERATE(range(-1, MAX_DEG));
    auto f = RandomPoly{}.get(f_deg);
    auto g = RandomPoly{}.get(g_deg);
    auto sum_fg = f + g;
    std::vector<Mod> expected_sum_fg(std::max(f_deg, g_deg) + 1);
    for (int i = 0; i <= std::max(f_deg, g_deg); ++i) {
      if (i <= f_deg) {
        expected_sum_fg[i] += f[i];
      }
      if (i <= g_deg) {
        expected_sum_fg[i] += g[i];
      }
    }
    for (int i = 0; i <= std::max(f_deg, g_deg); ++i) {
      REQUIRE(sum_fg[i].get() == expected_sum_fg[i].get());
    }
  }

  SECTION("subtracion") {
    auto sum_deg = GENERATE(range(0, MAX_DEG));
    auto f_deg = GENERATE(range(0, MAX_DEG));
    if (f_deg <= sum_deg) {
      auto g_deg = sum_deg - f_deg;
      auto f = RandomPoly{}.get(f_deg);
      auto g_coef = RandomPoly{}.get(g_deg).vector();
      auto neg_g_coef = g_coef;
      for (int i = 0; i <= g_deg; ++i) {
        neg_g_coef[i] = -neg_g_coef[i];
      }
      Poly g{g_coef}, neg_g{neg_g_coef};
      auto diff_fg = f - g;
      auto alt_diff_fg = f + neg_g;
      for (int i = 0; i <= std::max(f_deg, g_deg); ++i) {
        REQUIRE(diff_fg[i].get() == alt_diff_fg[i].get());
      }
    }
  }

  SECTION("multiplication") {
    auto sum_deg = GENERATE(range(0, MAX_DEG));
    auto f_deg = GENERATE(range(0, MAX_DEG));
    if (f_deg <= sum_deg) {
      auto g_deg = sum_deg - f_deg;
      auto f = RandomPoly{}.get(f_deg);
      auto g = RandomPoly{}.get(g_deg);
      auto g_coef = RandomPoly{}.get(g_deg).vector();
      auto neg_g_coef = g_coef;
      for (int i = 0; i <= g_deg; ++i) {
        neg_g_coef[i] = -neg_g_coef[i];
      }
      std::vector<Mod> expected_prod_fg(sum_deg + 1);
      for (int i = 0; i <= f_deg; ++i) {
        for (int j = 0; j <= g_deg; ++j) {
          expected_prod_fg[i + j] += f[i] * g[j];
        }
      }
      auto prod_fg = f * g;
      for (int i = 0; i <= sum_deg; ++i) {
        REQUIRE(prod_fg[i].get() == expected_prod_fg[i].get());
      }
      f *= g;
      for (int i = 0; i <= sum_deg; ++i) {
        REQUIRE(f[i].get() == expected_prod_fg[i].get());
      }
    }
  }

  SECTION("inverse") {
    auto f_deg = GENERATE(range(0, MAX_DEG));
    auto f = RandomPoly{}.get1(f_deg);
    PolyInv<Poly> inv{};
    auto inv_f = inv(f);
    REQUIRE(inv_f.deg() == f.deg());
    auto id = f * inv_f;
    id.resize(f_deg + 1);
    REQUIRE(id[0].get() == 1);
    for (int i = 1; i <= f_deg; ++i) {
      REQUIRE(id[i].get() == 0);
    }
  }

  SECTION("division") {
    auto f_deg = GENERATE(range(0, MAX_DEG));
    auto g_deg = GENERATE(range(0, MAX_DEG));
    auto f = RandomPoly{}.get(f_deg);
    auto g = RandomPoly{}.get1(g_deg);
    PolyDiv<Poly> div{};
    auto q = div(f, g);
    REQUIRE(q.deg() == std::max(f.deg(), g.deg()));
    auto gq = g * q;
    gq.resize(f.size());
    for (int i = 0; i <= f_deg; ++i) {
      REQUIRE(f[i].get() == gq[i].get());
    }
  }

  SECTION("logarithm") {
    auto f_deg = GENERATE(range(0, MAX_DEG));
    auto f = RandomPoly{}.get(f_deg);
    f[0] = Mod{1};
    PolyLog<Poly> log{};
    auto log_f = log(f);
    REQUIRE(log_f.deg() == f_deg);
    std::vector<Mod> expected_log_f(f_deg + 1);
    for (int i = 1; i <= f_deg; ++i) {
      expected_log_f[i] = Mod{i} * f[i];
      for (int j = 1; j <= i; ++j) {
        expected_log_f[i] -= f[j] * expected_log_f[i - j];
      }
    }
    for (int i = 1; i <= f_deg; ++i) {
      expected_log_f[i] *= Mod{i}.inv();
    }
    for (int i = 0; i <= f_deg; ++i) {
      REQUIRE(log_f[i].get() == expected_log_f[i].get());
    }
  }

  SECTION("exponentiation") {
    auto f_deg = GENERATE(range(0, MAX_DEG));
    auto f = RandomPoly{}.get(f_deg);
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
    for (int i = 0; i <= f_deg; ++i) {
      REQUIRE(exp_f[i].get() == expected_exp_f[i].get());
    }
  }

  SECTION("multi_eval") {
    auto f_deg = GENERATE(range(0, MAX_DEG));
    auto f = RandomPoly{}.get(f_deg).vector();
    auto m = GENERATE(0, MAX_DEG);
    auto a = RandomPoly{}.get(m - 1).vector();
    PolyMultiEval<Poly> eval;
    auto result = eval(f, a);
    for (int i = 0; i < m; ++i) {
      REQUIRE(result[i].get() == slow_eval(f, a[i]).get());
    }
  }
}
