#include "../ntt.h"
#include "../montgomery.h"

#include "gtest/gtest.h"

#include <random>

using namespace ntt;

static const int MOD = 998244353;
static const int N = 1 << 4;

using ModT = montgomery::Montgomery32T<MOD>;

TEST(NTT, Convolution) {
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

TEST(NTT, Inverse) {
  std::vector<ModT> p(N << 1), q(N << 1);
  std::mt19937 gen(0);
  for (int i = 0; i < N; ++i) {
    p[i] = ModT(gen() % MOD);
  }
  Inverse<NTT<ModT>> poly_inv(N);
  poly_inv(N, p.data(), q.data());
  std::vector<ModT> out(N << 1);
  NTT<ModT>::convolute(N << 1, p.data(), q.data(), out.data());
  for (int i = 0; i < N << 1; ++i) {
    std::cerr << out[i].get() << " \n"[i + 1 == (N << 1)];
  }
}
