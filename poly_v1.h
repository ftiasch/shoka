#pragma once

#include "ntt_util.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace ntt {

template <typename NTT> class PolyT_v1 {
protected:
  using Mod = typename NTT::Mod;

public:
  PolyT_v1(int max_n_) : max_n(max_n_), inv(max_n) {
    inv[1] = Mod(1);
    for (int i = 2; i < max_n; ++i) {
      inv[i] = Mod(Mod::MOD - Mod::MOD / i) * inv[Mod::MOD % i];
    }
    for (int i = 0; i < 4; ++i) {
      buffer[i].resize(max_n);
    }
  }

  void multiply(std::vector<Mod> &out, const std::vector<Mod> &f,
                const std::vector<Mod> &g) {
    Mod *b0 = buffer[0].data();
    Mod *b1 = buffer[1].data();

    int deg_plus_1 = f.size() + g.size() - 1;
    int n = min_power_of_two(deg_plus_1);
    assert_max_n(n);

    copy_and_fill0(n, b0, f.size(), f.data());
    NTT::dif(n, b0);
    copy_and_fill0(n, b1, g.size(), g.data());
    NTT::dif(n, b1);
    dot_product_and_dit(n, Mod(n).inverse(), b0, b0, b1);
    out.resize(deg_plus_1);
    for (int i = 0; i < deg_plus_1; ++i) {
      out[i] = b0[i];
    }
  }

  std::vector<Mod> multiply(const std::vector<Mod> &f,
                            const std::vector<Mod> &g) {
    std::vector<Mod> out;
    multiply(out, f, g);
    return out;
  }

  void inverse(int n, Mod *out, const Mod *f) {
    assert_power_of_two(n);
    assert_max_n(n);

    Mod *const b0 = buffer[0].data();
    Mod *const b1 = buffer[1].data();

    std::fill(out, out + n, Mod(0));
    out[0] = f[0].inverse();
    Mod inv_m(1);
    for (int m = 2; m <= n; m <<= 1) {
      copy_and_fill0(m, b0, n, f);
      NTT::dif(m, b0);
      std::copy(out, out + m, b1);
      NTT::dif(m, b1);
      inv_m *= Mod(2).inverse();
      dot_product_and_dit(m, inv_m, b0, b0, b1);
      std::fill(b0, b0 + (m >> 1), Mod(0));
      NTT::dif(m, b0);
      dot_product_and_dit(m, inv_m, b0, b0, b1);
      for (int i = m >> 1; i < m; ++i) {
        out[i] = Mod(0) - b0[i];
      }
    }
  }

  // f / g
  void divide(int n, Mod *out, const Mod *f, const Mod *g) {
    if (n == 1) {
      out[0] = f[0] * g[0].inverse();
    } else {
      assert_power_of_two(n);
      assert_max_n(n);

      Mod *const b0 = buffer[0].data();
      Mod *const b1 = buffer[1].data();
      Mod *const inv_g = buffer[2].data();

      int m = n >> 1;
      inverse(m, inv_g, g);
      std::fill(inv_g + m, inv_g + n, Mod(0));
      NTT::dif(n, inv_g);
      copy_and_fill0(n, b0, m, f);
      NTT::dif(n, b0);
      const Mod inv_n = Mod(n).inverse();
      dot_product_and_dit(n, inv_n, b0, b0, inv_g);

      std::copy(b0, b0 + m, out);

      std::fill(b0 + m, b0 + n, Mod(0));
      NTT::dif(n, b0);
      std::copy(g, g + n, b1);
      NTT::dif(n, b1);
      dot_product_and_dit(n, inv_n, b0, b0, b1);
      std::fill(b0, b0 + m, Mod(0));
      for (int i = m; i < n; ++i) {
        b0[i] -= f[i];
      }
      NTT::dif(n, b0);
      dot_product_and_dit(n, inv_n, b0, b0, inv_g);
      for (int i = m; i < n; ++i) {
        out[i] = Mod(0) - b0[i];
      }
    }
  }

  void log(int n, Mod *out, const Mod *f) {
    if (f[0].get() != 1) {
      throw std::invalid_argument("[x^0] f != 1");
    }
    assert_power_of_two(n);
    assert_max_n(n);
    // log f = \int f' / f
    Mod *const d_f = buffer[3].data();
    d_f[n - 1] = Mod(0);
    for (int i = 1; i < n; ++i) {
      d_f[i - 1] = Mod(i) * f[i];
    }
    divide(n, out, d_f, f);
    for (int i = n; i-- > 1;) {
      out[i] = out[i - 1] * inv[i];
    }
    out[0] = Mod(0);
  }

  void exp(int n, Mod *out, const Mod *f) {
    if (f[0].get() != 0) {
      throw std::invalid_argument("[x^0] f != 0");
    }
    if (n == 1) {
      out[0] = Mod(1);
    } else {
      assert_power_of_two(n);
      assert_max_n(n);

      Mod *const b0 = buffer[0].data();
      Mod *const b1 = buffer[1].data();
      Mod *const b2 = buffer[2].data();
      Mod *const b3 = buffer[3].data();

      out[0] = b1[0] = b1[1] = b2[0] = Mod(1);
      Mod inv_m(1);
      for (int m = 1; m < (n >> 1); m <<= 1) {
        const Mod inv_2m = inv_m * Mod(2).inverse();
        for (int i = 0; i < m; ++i) {
          b0[i] = Mod(i) * f[i];
        }
        NTT::dif(m, b0);
        dot_product_and_dit(m, inv_m, b0, b0, b1);
        for (int i = 0; i < m; ++i) {
          b0[i] -= Mod(i) * out[i];
        }
        std::fill(b0 + m, b0 + (m << 1), Mod(0));
        NTT::dif(m << 1, b0);
        copy_and_fill0(m << 1, b3, m, b2);
        NTT::dif(m << 1, b3);
        dot_product_and_dit(m << 1, inv_2m, b0, b0, b3);
        for (int i = 0; i < m; ++i) {
          b0[i] = b0[i] * inv[m + i] + f[m + i];
        }
        std::fill(b0 + m, b0 + (m << 1), Mod(0));
        NTT::dif(m << 1, b0);
        dot_product_and_dit(m << 1, inv_2m, b0, b0, b1);
        std::copy(b0, b0 + m, out + m);
        copy_and_fill0(m << 2, b1, m << 1, out);
        NTT::dif(m << 2, b1);
        dot_product_and_dit(m << 1, inv_2m, b0, b1, b3);
        std::fill(b0, b0 + m, Mod(0));
        NTT::dif(m << 1, b0);
        dot_product_and_dit(m << 1, inv_2m, b0, b0, b3);
        for (int i = m; i < m << 1; ++i) {
          b2[i] = Mod(0) - b0[i];
        }
        inv_m = inv_2m;
      }
      const int m = n >> 1;
      for (int i = 0; i < m; ++i) {
        b0[i] = Mod(i) * f[i];
      }
      NTT::dif(m, b0);
      dot_product_and_dit(m, inv_m, b0, b0, b1);
      for (int i = 0; i < m; ++i) {
        b0[i] -= Mod(i) * out[i];
      }
      copy_and_fill0(m, b0 + m, m >> 1, b0 + (m >> 1));
      std::fill(b0 + (m >> 1), b0 + m, Mod(0));
      NTT::dif(m, b0);
      NTT::dif(m, b0 + m);
      copy_and_fill0(m, b3 + m, m >> 1, b2 + (m >> 1));
      NTT::dif(m, b3 + m);
      for (int i = 0; i < m; ++i) {
        b0[m + i] = inv_m * (b0[i] * b3[m + i] + b0[m + i] * b3[i]);
      }
      dot_product_and_dit(m, inv_m, b0, b0, b3);
      NTT::dit(m, b0 + m);
      for (int i = 0; i < m >> 1; ++i) {
        b0[(m >> 1) + i] += b0[m + i];
      }
      for (int i = 0; i < m; ++i) {
        b0[i] = b0[i] * inv[m + i] + f[m + i];
      }
      std::fill(b0 + m, b0 + (m << 1), Mod(0));
      NTT::dif(m << 1, b0);
      const Mod inv_2m = inv_m * Mod(2).inverse();
      dot_product_and_dit(m << 1, inv_2m, b0, b0, b1);
      std::copy(b0, b0 + m, out + m);
    }
  }

protected:
  static void copy_and_fill0(int n, Mod *dst, int m, const Mod *src) {
    m = std::min(n, m);
    std::copy(src, src + m, dst);
    std::fill(dst + m, dst + n, Mod(0));
  }

  static void dot_product_and_dit(int n, Mod inv_n, Mod *out, const Mod *a,
                                  const Mod *b) {
    for (int i = 0; i < n; ++i) {
      out[i] = inv_n * a[i] * b[i];
    }
    NTT::dit(n, out);
  }

  void assert_max_n(int n) const {
    if (n > max_n) {
      throw std::invalid_argument("insufficient buffer");
    }
  }

  int max_n;
  std::vector<Mod> inv;
  std::array<std::vector<Mod>, 4> buffer;
};

} // namespace ntt
