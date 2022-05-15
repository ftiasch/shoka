#include "../montgomery.h"
#include "../multi_eval.h"

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


static const int K = 5;

ntt::MultiEval<ntt::NTT<ModT>> poly(1 << K);

TEST(MultiEval, Correctness) {
  for (int _ = 0; _ < 10; ++_) {
    int n = mt() % (1 << K) + 1;
    int m = mt() % (1 << K) + 1;
    std::vector<ModT> c = random_poly(n);
    std::vector<ModT> a = random_poly(m);
    auto result = poly.eval(c, a);
    ASSERT_EQ(result.size(), m);
    for (int i = 0; i < m; ++i) {
      ModT x = a[i];
      ModT answer{0};
      for (int j = n; j--;) {
        answer = answer * x + c[j];
      }
      ASSERT_EQ(result[i].get(), answer.get());
    }
  }
}
