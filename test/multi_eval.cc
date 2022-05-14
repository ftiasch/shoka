#include "../montgomery.h"
#include "../ntt.h"

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

static const int K = 10;

using NTT = ntt::NTT<ModT>;
ntt::Poly<NTT> poly(1 << K);

std::vector<ModT> brute_middle_product(const std::vector<ModT> &a,
                                       const std::vector<ModT> &c) {
  int n = a.size();
  assert(c.size() == (n << 1));
  std::vector<ModT> b(n);
  for (int i = n; i--;) {
    for (int j = n; j--;) {
      b[j] += a[i] * c[i + j];
    }
  }
  return b;
}

TEST(MultiEval, MiddleProduct) {
  int n = 1 << (K - 1);
  std::vector<ModT> a = random_poly(n);
  std::vector<ModT> c = random_poly(n << 1);
  c[(n << 1) - 1] = ModT{0};
  std::vector<ModT> brute_b = brute_middle_product(a, c);
  std::reverse(a.begin(), a.end());
  a.resize(n << 1);
  NTT::dif(n << 1, a.data());
  NTT::dif(n << 1, c.data());
  ModT inv_2n = ModT(n << 1).inverse();
  for (int i = 0; i < n << 1; ++i) {
    a[i] = inv_2n * a[i] * c[i];
  }
  NTT::dit(n << 1, a.data());
  for (int i = 0; i < n; ++i) {
    ASSERT_EQ(brute_b[i].get(), a[n - 1 + i].get());
  }
}
