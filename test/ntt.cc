#include "../ntt.h"
#include "../mod.h"

#include "gtest/gtest.h"

#include <random>

using namespace ntt;

static const int MOD = 998244353;
static const int N = 1 << 8;

using ModT = montgomery::Montgomery32T<MOD>;
// using ModT = mod::ModT<MOD>;

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
  std::mt19937 gen(0);
  Inverse<NTT<ModT>> poly_inv(N);
  for (int _ = 0; _ < 2; ++_) {
    std::vector<ModT> p(N << 1), q(N << 1);
    p[0] = ModT(gen() % (MOD - 1) + 1);
    for (int i = 1; i < N; ++i) {
      p[i] = ModT(gen() % MOD);
    }
    poly_inv(N, p.data(), q.data());
    std::vector<ModT> out(N << 1);
    NTT<ModT>::convolute(N << 1, p.data(), q.data(), out.data());
    ASSERT_EQ(out[0].get(), 1);
    for (int i = 1; i < N; ++i) {
      ASSERT_EQ(out[i].get(), 0);
    }
  }
}

TEST(NTT, Divide) {
  std::mt19937 gen(0);
  Divide<NTT<ModT>> poly_div(N);
  for (int _ = 0; _ < 2; ++_) {
    std::vector<ModT> p(N << 1), q(N << 1), r(N << 1);
    for (int i = 0; i < N; ++i) {
      p[i] = ModT(gen() % MOD);
      q[i] = ModT(gen() % MOD);
    }
    q[0] = ModT(gen() % (MOD - 1) + 1);
    poly_div(N, p.data(), q.data(), r.data());
    std::vector<ModT> out(N << 1);
    NTT<ModT>::convolute(N << 1, q.data(), r.data(), out.data());
    for (int i = 0; i < N; ++i) {
      ASSERT_EQ(out[i].get(), p[i].get());
    }
  }
}
