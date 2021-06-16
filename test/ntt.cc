#include "../ntt.h"
#include "../montgomery.h"

#include "gtest/gtest.h"

#include <random>

static const int MOD = 998244353;
static const int N = 1 << 8;

TEST(NTT, Convolution) {
  using ModT = montgomery::Montgomery32T<MOD>;

  std::vector<ModT> a(N), b(N), answer(N);
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

  {
    std::vector<ModT> copy_a(a), copy_b(b), out(N);
    NTT<ModT> ntt(N);
    ntt.convolute(copy_a.data(), copy_b.data(), out.data());
    for (int i = 0; i < N; ++i) {
      ASSERT_EQ(answer[i].get(), out[i].get());
    }
  }

  {
    std::vector<ModT> copy_a(a), copy_b(b), out(N);
    NTT<ModT> ntt(N);
    NTT<ModT>::convolute(N, copy_a.data(), copy_b.data(), out.data());
    for (int i = 0; i < N; ++i) {
      ASSERT_EQ(answer[i].get(), out[i].get());
    }
  }
}
