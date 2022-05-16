#include "../multi_eval.h"
#include "../montgomery.h"

#include "../debug.h"

#include "gtest/gtest.h"

#include <algorithm>
#include <cassert>
#include <random>

static const int MOD = 998244353;

using ModT = montgomery::Montgomery32T<MOD>;

std::mt19937_64 mt;

std::vector<ModT> random_poly(int n) {
  std::vector<ModT> result(n);
  for (int i = 0; i < n; ++i) {
    result[i] = ModT(mt() % MOD);
  }
  return result;
}

ModT slow_eval(const std::vector<ModT> &c, ModT x) {
  ModT result{0};
  for (int i = c.size(); i--;) {
    result = result * x + c[i];
  }
  return result;
}

static const int K = 10;

ntt::MultiEval<ntt::NTT<ModT>> poly(1 << K);

TEST(MultiEval, Correctness) {
  for (int _ = 0; _ < 1; ++_) {
    // int n = mt() % (1 << K) + 1;
    // int m = mt() % (1 << K) + 1;
    int n = 1 << K;
    int m = 1 << K;
    std::vector<ModT> c(n), a(n);
    for (int i = 0; i < n; ++i) {
      c[i] = a[i] = ModT(i + 1);
    }
    auto result = poly.eval(c, a);
    ASSERT_EQ(result.size(), m);
    for (int i = 0; i < m; ++i) {
      ASSERT_EQ(result[i].get(), slow_eval(c, a[i]).get()) << "i=" << i;
    }
  }
}

TEST(MultiEval, MultiSizedCorrectness) {
  for (int n = 0; n <= 10; ++n) {
    for (int m = 0; m <= 10; ++m) {
      std::vector<ModT> c = random_poly(n);
      std::vector<ModT> a = random_poly(m);
      auto result = poly.eval(c, a);
      ASSERT_EQ(result.size(), m);
      for (int i = 0; i < m; ++i) {
        ASSERT_EQ(result[i].get(), slow_eval(c, a[i]).get()) << "n=" << n << "|m=" << m << "|i=" << i;
      }
    }
  }
}
