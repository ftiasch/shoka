#pragma once

#include "ntt.h"
#include "singleton.h"
#include "snippets/min_pow_of_two.h"

#include <cstdlib>
#include <vector>

template <typename Mod> struct PolyConv {
  using Vector = std::vector<Mod>;

  void operator()(Vector &out, const Vector &lhs, const Vector &rhs,
                  int limit = std::numeric_limits<int>::max()) const {
    int deg_plus_1 = std::min(limit, static_cast<int>(lhs.size()) +
                                         static_cast<int>(rhs.size()) - 1);
    if (deg_plus_1 <= 16) {
      out.assign(deg_plus_1, Mod{0});
      for (int i = 0; i < lhs.size(); ++i) {
        for (int j = 0; j < rhs.size() && i + j < limit; ++j) {
          out[i + j] += lhs[i] * rhs[j];
        }
      }
    }
    int n = min_pow_of_two(deg_plus_1);
    ntt().reserve(n);
    Mod *b0 = ntt().template raw_buffer<0>();
    Mod *b1 = ntt().template raw_buffer<1>();
    copy_and_fill0(n, b0, lhs);
    ntt().dif(n, b0);
    copy_and_fill0(n, b1, rhs);
    ntt().dif(n, b1);
    out.resize(n);
    auto inv_n = ntt().power_of_two_inv(n);
    for (int i = 0; i < n; ++i) {
      out[i] = inv_n * b0[i] * b1[i];
    }
    ntt().dit(n, out.data());
    out.resize(deg_plus_1);
  }

private:
  using Ntt = NttT<Mod>;

  static Ntt &ntt() { return singleton<Ntt>(); }

  static void copy_and_fill0(int n, Mod *dst, int m, const Mod *src) {
    m = std::min(n, m);
    std::copy(src, src + m, dst);
    std::fill(dst + m, dst + n, Mod{0});
  }

  static void copy_and_fill0(int n, Mod *dst, const std::vector<Mod> &src) {
    copy_and_fill0(n, dst, src.size(), src.data());
  }
};
