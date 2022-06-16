#include "../poly.h"
#include "../montgomery.h"
#include "../ntt.h"
#include "../poly_div.h"
#include "../poly_exp.h"
#include "../poly_inv.h"
#include "../poly_log.h"
#include "../poly_multieval.h"

#include "../debug.h"

#include "gtest/gtest.h"

#include <memory>
#include <random>

using Mod = montgomery::Montgomery32T<998'244'353>;
using NTT = ntt::NTT<Mod>;

class PolyTest : public ::testing::Test {
protected:
  static const int MAX_DEG = 50;

  std::vector<Mod> gen_poly(int deg) {
    std::vector<Mod> result(deg + 1);
    for (int i = 0; i <= deg; ++i) {
      result[i] = Mod(gen() % Mod::MOD);
    }
    return result;
  }

  std::vector<Mod> gen_poly1(int deg) {
    std::vector<Mod> result(deg + 1);
    result[0] = Mod(gen() % (Mod::MOD - 1) + 1);
    for (int i = 1; i <= deg; ++i) {
      result[i] = Mod(gen() % Mod::MOD);
    }
    return result;
  }

private:
  std::mt19937_64 gen;
};

Mod slow_eval(const std::vector<Mod> &c, Mod x) {
  Mod result{0};
  for (int i = c.size(); i--;) {
    result = result * x + c[i];
  }
  return result;
}

TEST_F(PolyTest, Addition) {
  for (int sum_deg = 0; sum_deg <= MAX_DEG; ++sum_deg) {
    auto factory = PolyFactory<NTT>::create();
    for (int f_deg = 0; f_deg <= sum_deg; ++f_deg) {
      int g_deg = sum_deg - f_deg;
      auto f = factory->make(gen_poly(f_deg));
      auto g = factory->make(gen_poly(g_deg));
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

TEST_F(PolyTest, Subtraction) {
  for (int sum_deg = 0; sum_deg <= MAX_DEG; ++sum_deg) {
    auto factory = PolyFactory<NTT>::create();
    for (int f_deg = 0; f_deg <= sum_deg; ++f_deg) {
      int g_deg = sum_deg - f_deg;
      auto f = factory->make(gen_poly(f_deg));
      auto g_coef = gen_poly(g_deg);
      auto neg_g_coef = g_coef;
      for (int i = 0; i <= g_deg; ++i) {
        neg_g_coef[i] = -neg_g_coef[i];
      }
      auto g = factory->make(g_coef);
      auto neg_g = factory->make(neg_g_coef);

      auto diff_fg = f - g;
      auto alt_diff_fg = f + neg_g;
      for (int i = 0; i <= std::max(f_deg, g_deg); ++i) {
        ASSERT_EQ(diff_fg[i].get(), alt_diff_fg[i].get());
      }
    }
  }
}

TEST_F(PolyTest, Multiplication) {
  for (int sum_deg = 0; sum_deg <= MAX_DEG; ++sum_deg) {
    auto factory = PolyFactory<NTT>::create();
    for (int f_deg = 0; f_deg <= sum_deg; ++f_deg) {
      int g_deg = sum_deg - f_deg;
      auto f = factory->make(gen_poly(f_deg));
      auto g = factory->make(gen_poly(g_deg));
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

TEST_F(PolyTest, InplaceMultiplication) {
  for (int sum_deg = 0; sum_deg <= MAX_DEG; ++sum_deg) {
    auto factory = PolyFactory<NTT>::create();
    for (int f_deg = 0; f_deg <= sum_deg; ++f_deg) {
      int g_deg = sum_deg - f_deg;
      auto f = factory->make(gen_poly(f_deg));
      auto g = factory->make(gen_poly(g_deg));
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

TEST_F(PolyTest, Inverse) {
  for (int f_deg = 0; f_deg <= MAX_DEG; ++f_deg) {
    auto factory = PolyFactory<NTT>::create();
    auto f = factory->make(gen_poly1(f_deg));
    PolyInv<NTT> inv{factory};
    auto f_inv = inv(f);
    ASSERT_EQ(f_inv.size(), f.size());
    auto id = f * f_inv;
    id.resize(f_deg + 1);
    ASSERT_EQ(id[0].get(), 1);
    for (int i = 1; i <= f_deg; ++i) {
      ASSERT_EQ(id[i].get(), 0);
    }
  }
}

TEST_F(PolyTest, Division) {
  for (int f_deg = 0; f_deg <= MAX_DEG; ++f_deg) {
    for (int g_deg = 0; g_deg <= MAX_DEG; ++g_deg) {
      auto factory = PolyFactory<NTT>::create();
      auto f = factory->make(gen_poly(f_deg));
      auto g = factory->make(gen_poly1(g_deg));
      PolyDiv<NTT> div{factory};
      auto q = div(f, g);
      ASSERT_EQ(q.size(), std::max(f.size(), g.size()));
      auto gq = g * q;
      gq.resize(f.size());
      for (int i = 0; i <= f_deg; ++i) {
        ASSERT_EQ(f[i].get(), gq[i].get());
      }
    }
  }
}

TEST_F(PolyTest, Logarithm) {
  for (int f_deg = 0; f_deg <= MAX_DEG; ++f_deg) {
    auto factory = PolyFactory<NTT>::create();
    auto f = factory->make(gen_poly(f_deg));
    f[0] = Mod{1};
    PolyLog<NTT> log{factory};
    auto log_f = log(f);
    ASSERT_EQ(log_f.deg(), f_deg);
    std::vector<Mod> answer(f_deg + 1, Mod{0});
    for (int i = 1; i <= f_deg; ++i) {
      answer[i] = Mod(i) * f[i];
      for (int j = 1; j <= i; ++j) {
        answer[i] -= f[j] * answer[i - j];
      }
    }
    for (int i = 1; i <= f_deg; ++i) {
      answer[i] *= Mod(i).inverse();
    }
    for (int i = 0; i <= f_deg; ++i) {
      ASSERT_EQ(log_f[i].get(), answer[i].get());
    }
  }
}

TEST_F(PolyTest, Exponentiation) {
  for (int f_deg = 0; f_deg <= MAX_DEG; ++f_deg) {
    auto factory = PolyFactory<NTT>::create();
    auto f = factory->make(gen_poly(f_deg));
    f[0] = Mod{0};
    PolyExp<NTT> exp{factory};
    auto exp_f = exp(f);
    ASSERT_EQ(exp_f.deg(), f_deg);
    std::vector<Mod> answer(f_deg + 1, Mod{0}), g(f_deg + 1);
    for (int i = 1; i <= f_deg; ++i) {
      g[i] = Mod(i) * f[i];
    }
    answer[0] = Mod(1);
    for (int i = 1; i <= f_deg; ++i) {
      for (int j = 0; j < i; ++j) {
        answer[i] += g[j + 1] * answer[i - 1 - j];
      }
      answer[i] *= Mod(i).inverse();
    }
    for (int i = 0; i <= f_deg; ++i) {
      ASSERT_EQ(exp_f[i].get(), answer[i].get());
    }
  }
}

TEST_F(PolyTest, MultiEvaluation) {
  for (int f_deg = 0; f_deg <= MAX_DEG; ++f_deg) {
    auto factory = PolyFactory<NTT>::create();
    PolyMultiEval<NTT> multi_eval(factory);
    auto f = gen_poly(f_deg);
    for (int m = 0; m <= MAX_DEG; ++m) {
      std::vector<Mod> a = gen_poly(m - 1);
      auto result = multi_eval(f, a);
      for (int i = 0; i < m; ++i) {
        ASSERT_EQ(result[i].get(), slow_eval(f, a[i]).get());
      }
    }
  }
}
