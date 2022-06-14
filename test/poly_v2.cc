#include "../finite_poly.h"
#include "../montgomery.h"
#include "../ntt.h"

#include "../debug.h"

#include "gtest/gtest.h"

#include <memory>
#include <random>

using Mod = montgomery::Montgomery32T<998'244'353>;
using NTT = ntt::NTT<Mod>;

class PolyV2Test : public ::testing::Test {
protected:
  static const int MAX_DEG = 30;

  std::vector<Mod> gen_poly(int deg) {
    std::vector<Mod> result(deg + 1);
    for (int i = 0; i <= deg; ++i) {
      result[i] = Mod(gen() % Mod::MOD);
    }
    return result;
  }

private:
  std::mt19937_64 gen;
};

TEST_F(PolyV2Test, Addition) {
  for (int sum_deg = 0; sum_deg <= MAX_DEG; ++sum_deg) {
    ntt::FinitePolyFactoryT<NTT> factory(sum_deg);
    for (int f_deg = 0; f_deg <= sum_deg; ++f_deg) {
      int g_deg = sum_deg - f_deg;
      auto f = factory.make_poly(gen_poly(f_deg));
      auto g = factory.make_poly(gen_poly(g_deg));
      std::vector<Mod> correct_sum_fg(std::max(f_deg, g_deg) + 1);
      for (int i = 0; i <= std::max(f_deg, g_deg); ++i) {
        if (i <= f_deg) {
          correct_sum_fg[i] += f[i];
        }
        if (i <= g_deg) {
          correct_sum_fg[i] += g[i];
        }
      }
      auto sum_fg = f + g;
      for (int i = 0; i <= std::max(f_deg, g_deg); ++i) {
        ASSERT_EQ(sum_fg[i].get(), correct_sum_fg[i].get());
      }
    }
  }
}

TEST_F(PolyV2Test, Subtraction) {
  for (int sum_deg = 0; sum_deg <= MAX_DEG; ++sum_deg) {
    ntt::FinitePolyFactoryT<NTT> factory(sum_deg);
    for (int f_deg = 0; f_deg <= sum_deg; ++f_deg) {
      int g_deg = sum_deg - f_deg;
      auto f = factory.make_poly(gen_poly(f_deg));
      auto g_coef = gen_poly(g_deg);
      auto neg_g_coef = g_coef;
      for (int i = 0; i <= g_deg; ++i) {
        neg_g_coef[i] = -neg_g_coef[i];
      }
      auto g = factory.make_poly(g_coef);
      auto neg_g = factory.make_poly(neg_g_coef);

      auto diff_fg = f - g;
      auto alt_diff_fg = f + neg_g;
      for (int i = 0; i <= std::max(f_deg, g_deg); ++i) {
        ASSERT_EQ(diff_fg[i].get(), alt_diff_fg[i].get());
      }
    }
  }
}

TEST_F(PolyV2Test, Multiplication) {
  for (int sum_deg = 0; sum_deg <= MAX_DEG; ++sum_deg) {
    ntt::FinitePolyFactoryT<NTT> factory(sum_deg);
    for (int f_deg = 0; f_deg <= sum_deg; ++f_deg) {
      int g_deg = sum_deg - f_deg;
      auto f = factory.make_poly(gen_poly(f_deg));
      auto g = factory.make_poly(gen_poly(g_deg));
      std::vector<Mod> correct_fg(sum_deg + 1);
      for (int i = 0; i <= f_deg; ++i) {
        for (int j = 0; j <= g_deg; ++j) {
          correct_fg[i + j] += f[i] * g[j];
        }
      }
      auto fg = f * g;
      for (int i = 0; i <= sum_deg; ++i) {
        ASSERT_EQ(fg[i].get(), correct_fg[i].get());
      }
    }
  }
}

TEST_F(PolyV2Test, InplaceMultiplication) {
  for (int sum_deg = 0; sum_deg <= MAX_DEG; ++sum_deg) {
    ntt::FinitePolyFactoryT<NTT> factory(sum_deg);
    for (int f_deg = 0; f_deg <= sum_deg; ++f_deg) {
      int g_deg = sum_deg - f_deg;
      auto f = factory.make_poly(gen_poly(f_deg));
      auto g = factory.make_poly(gen_poly(g_deg));
      std::vector<Mod> correct_fg(sum_deg + 1);
      for (int i = 0; i <= f_deg; ++i) {
        for (int j = 0; j <= g_deg; ++j) {
          correct_fg[i + j] += f[i] * g[j];
        }
      }
      f *= g;
      for (int i = 0; i <= sum_deg; ++i) {
        ASSERT_EQ(f[i].get(), correct_fg[i].get());
      }
    }
  }
}
