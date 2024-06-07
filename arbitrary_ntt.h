#pragma once

#include "snippets/min_pow_of_two.h"

#include <array>
#include <cmath>
#include <complex>
#include <vector>

template <typename Mod> struct ArbitraryNTT {
  std::vector<Mod> operator()(const std::vector<Mod> &a_,
                              const std::vector<Mod> &b_) {
    int deg_plus_1 = a_.size() + b_.size() - 1;
    int n = min_pow_of_two(deg_plus_1);
    if (max_n < n) {
      max_n = n;
      twiddles.resize(max_n + 1);
      static const double PI = acos(-1);
      for (int i = 0; i <= max_n; i++) {
        twiddles[i] = Complex{cos(2 * i * PI / max_n), sin(2 * i * PI / max_n)};
      }
      for (int i = 0; i < NUMBER_OF_BUFFER; i++) {
        buffers[i].resize(max_n);
      }
    }
    auto a = buffers[0].data();
    auto b = buffers[1].data();
    auto c = buffers[2].data();
    auto d = buffers[3].data();
    radix_split(n, a, a_);
    radix_split(n, b, b_);
    dif(n, a);
    dif(n, b);
    for (int m = 1; m <= n; m <<= 1) {
      for (int i = m >> 1; i < m; i++) {
        auto coef = .5 * b[i];
        auto j = m + (m >> 1) - 1 - i;
        auto t = a[i] - std::conj(a[j]);
        c[i] = coef * Complex{t.imag(), -t.real()};
        d[i] = coef * (a[i] + std::conj(a[j]));
      }
    }
    dit(n, c);
    dit(n, d);
    std::vector<Mod> out(deg_plus_1);
    Mod M{1 << B}, M2{M * M};
    for (int i = 0; i < deg_plus_1; ++i) {
      auto da = Mod::normalize(static_cast<int64_t>(c[i].imag() / n + 0.5));
      auto db = Mod::normalize(static_cast<int64_t>(c[i].real() / n + 0.5));
      auto dc = Mod::normalize(static_cast<int64_t>(d[i].imag() / n + 0.5));
      auto dd = Mod::normalize(static_cast<int64_t>(d[i].real() / n + 0.5));
      out[i] = dd * M2 + (db + dc) * M + da;
    }
    return out;
  }

private:
  using Complex = std::complex<double>;

  static constexpr int B = 15;
  static constexpr int NUMBER_OF_BUFFER = 4;

  void radix_split(int n, Complex *b, const std::vector<Mod> &a) {
    for (int i = 0; i < a.size(); i++) {
      auto got = a[i].get();
      b[i] = Complex{static_cast<double>(got >> B),
                     static_cast<double>(got & ((1 << B) - 1))};
    }
    std::fill(b + a.size(), b + n, Complex{});
  }

  void dit(int n, Complex *a) {
    for (int m = 1; m < n; m <<= 1) {
      auto step = max_n / (m << 1);
      for (int i = 0; i < n; i += m << 1) {
        int tid = 0;
        for (int r = i; r < i + m; r++) {
          auto tmp = twiddles[tid] * a[r + m];
          a[r + m] = a[r];
          a[r + m] -= tmp;
          a[r] += tmp;
          tid += step;
        }
      }
    }
  }

  void dif(int n, Complex *a) {
    for (int m = n; m >>= 1;) {
      auto step = max_n / (m << 1);
      for (int i = 0; i < n; i += m << 1) {
        int tid = max_n;
        for (int r = i; r < i + m; r++) {
          auto tmp = a[r];
          tmp -= a[r + m];
          a[r] += a[r + m];
          a[r + m] = twiddles[tid] * tmp;
          tid -= step;
        }
      }
    }
  }

  int max_n = 0;
  std::vector<Complex> twiddles;
  std::array<std::vector<Complex>, NUMBER_OF_BUFFER> buffers;
};
