#include "../ntt.h"
#include "../montgomery.h"

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

std::vector<ModT> random_poly1(int n) {
  std::vector<ModT> result(n);
  result[0] = ModT(1 + mt() % (MOD - 1));
  for (int i = 1; i < n; ++i) {
    result[i] = ModT(mt() % MOD);
  }
  return result;
}

static const int K = 10;

ntt::Poly<ntt::NTT<ModT>> poly(1 << K);

TEST(NTT, Multiplication) {
  for (int k = K; k >= 1; --k) {
    std::vector<ModT> f = random_poly(1 << (k - 1));
    std::vector<ModT> g = random_poly(1 << (k - 1));
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
}

TEST(NTT, Inversion) {
  for (int k = K; k >= 0; --k) {
    const int n = 1 << k;
    std::vector<ModT> f = random_poly1(n);
    std::vector<ModT> inv_f(n);
    poly.inverse(n, inv_f.data(), f.data());

    std::vector<ModT> answer(n);
    answer[0] = f[0].inverse();
    for (int i = 1; i < n; ++i) {
      answer[i] = ModT(0);
      for (int j = 0; j < i; ++j) {
        answer[i] -= f[i - j] * answer[j];
      }
      answer[i] *= answer[0];
    }
    for (int i = 0; i < n; ++i) {
      ASSERT_EQ(inv_f[i].get(), answer[i].get());
    }
  }
}

// // TODO: Remainder

TEST(NTT, Division) {
  for (int k = K; k >= 0; --k) {
    const int n = 1 << k;
    std::vector<ModT> f = random_poly(n);
    std::vector<ModT> g = random_poly1(n);
    std::vector<ModT> quotient(n);
    poly.divide(n, quotient.data(), f.data(), g.data());

    std::vector<ModT> answer(n);
    const ModT inv_g0 = g[0].inverse();
    for (int i = 0; i < n; ++i) {
      answer[i] = f[i];
      for (int j = 0; j < i; ++j) {
        answer[i] -= g[i - j] * answer[j];
      }
      answer[i] *= inv_g0;
    }
    for (int i = 0; i < n; ++i) {
      ASSERT_EQ(quotient[i].get(), answer[i].get());
    }
  }
}

TEST(NTT, Logarithm) {
  for (int k = K; k >= 0; --k) {
    const int n = 1 << k;
    std::vector<ModT> f = random_poly(n);
    f[0] = ModT(1);
    std::vector<ModT> log_f(n);
    poly.log(n, log_f.data(), f.data());

    std::vector<ModT> answer(n, ModT(0));
    for (int i = 1; i < n; ++i) {
      answer[i] = ModT(i) * f[i];
      for (int j = 1; j <= i; ++j) {
        answer[i] -= f[j] * answer[i - j];
      }
    }
    for (int i = 1; i < n; ++i) {
      answer[i] *= ModT(i).inverse();
    }
    for (int i = 0; i < n; ++i) {
      ASSERT_EQ(log_f[i].get(), answer[i].get());
    }
  }
}

TEST(NTT, Exponentiation) {
  for (int k = K; k >= 0; --k) {
    const int n = 1 << k;
    std::vector<ModT> f = random_poly(n);
    f[0] = ModT(0);
    std::vector<ModT> exp_f(n);
    poly.exp(n, exp_f.data(), f.data());

    std::vector<ModT> answer(n, ModT(0)), g(n);
    for (int i = 1; i < n; ++i) {
      g[i] = ModT(i) * f[i];
    }
    answer[0] = ModT(1);
    for (int i = 1; i < n; ++i) {
      for (int j = 0; j < i; ++j) {
        answer[i] += g[j + 1] * answer[i - 1 - j];
      }
      answer[i] *= ModT(i).inverse();
    }
    for (int i = 0; i < n; ++i) {
      ASSERT_EQ(exp_f[i].get(), answer[i].get()) << "k=" << k << "|i=" << i;
    }
  }
}
