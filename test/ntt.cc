#include "../ntt.h"
#include "../montgomery.h"

#include "gtest/gtest.h"

#include <random>

static const int MOD = 998244353;
static const int N = 1 << 14;

TEST(NTT, Test) {
  using ModT = montgomery::Montgomery32T<MOD>;

  NTT<ModT> ntt(N);

  std::vector<ModT> a(N), b(N), answer(N), out(N);
  std::mt19937 gen(0);
  for (int i = 0; i < N; ++i) {
    a[i] = ModT(gen() % MOD);
    b[i] = ModT(gen() % MOD);
  }
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      answer[(i + j) % N] += a[i] * b[j];
    }
  }
  ntt.convolute(a.data(), b.data(), out.data());
  for (int i = 0; i < N; ++i) {
    ASSERT_EQ(answer[i].get(), out[i].get());
  }
}
