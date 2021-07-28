#include "../ntt.h"
#include "../mod.h"

#include "gtest/gtest.h"

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

static const int n = (1 << 10) - 233;

ntt::Poly<ntt::NTT<ModT>> poly(1 << 10);

TEST(NTT, Multiplication) {
  std::vector<ModT> f = random_poly((1 << 9) - 1);
  std::vector<ModT> g = random_poly((1 << 9) - 2);
  std::vector<ModT> fg;
  poly.multiply(fg, f, g);

  std::vector<ModT> answer(f.size() + g.size() - 1);
  for (int i = 0; i < f.size(); ++i) {
    for (int j = 0; j < g.size(); ++j) {
      answer[i + j] += f[i] * g[j];
    }
  }
  ASSERT_EQ(fg.size(), answer.size());
  for (int i = 0; i < answer.size(); ++i) {
    ASSERT_EQ(fg[i].get(), answer[i].get());
  }
}
