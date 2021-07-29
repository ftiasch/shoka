#pragma once

#include "mod.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <vector>

namespace ntt {

// using u32 = uint32_t;
// using u64 = uint64_t;

static void assert_power_of_two(int n) {
  if (n & (n - 1)) {
    throw std::invalid_argument(std::to_string(n) + " is not a power of two");
  }
}

template <typename ModT_> struct NTT {
  using ModT = ModT_;

  static void dit(int n, ModT *a) {
    assert_power_of_two(n);
    for (int m = 1; m < n; m <<= 1) {
      const ModT root = G.power((ModT::MOD - 1) / (m << 1));
      for (int i = 0; i < n; i += m << 1) {
        ModT twiddle(1);
        for (int r = i; r < i + m; ++r) {
          ModT tmp = twiddle * a[r + m];
          a[r + m] = a[r];
          a[r + m] -= tmp;
          a[r] += tmp;
          twiddle *= root;
        }
      }
    }
  }

  static void dif(int n, ModT *a) {
    assert_power_of_two(n);
    for (int m = n; m >>= 1;) {
      const ModT root = G.power(ModT::MOD - 1 - (ModT::MOD - 1) / (m << 1));
      for (int i = 0; i < n; i += m << 1) {
        ModT twiddle(1);
        for (int r = i; r < i + m; ++r) {
          ModT tmp = a[r];
          tmp -= a[r + m];
          a[r] += a[r + m];
          a[r + m] = twiddle * tmp;
          twiddle *= root;
        }
      }
    }
  }

private:
  struct FiniteField {
    static constexpr ModT primitive_root() {
      int g = 2;
      while (!is_primitive_root(ModT(g))) {
        g++;
      }
      return ModT(g);
    }

  private:
    static constexpr bool is_primitive_root(ModT g) {
      for (int d = 2; d * d <= ModT::MOD - 1; ++d) {
        if ((ModT::MOD - 1) % d == 0 &&
            (g.power(d).get() == 1 ||
             g.power((ModT::MOD - 1) / d).get() == 1)) {
          return false;
        }
      }
      return true;
    }
  };

  static constexpr ModT G = ModT(FiniteField::primitive_root());
};

template <typename NTT> class Poly {
private:
  using ModT = typename NTT::ModT;

public:
  Poly(int max_n_)
      : max_n(max_n_), inv(max_n), buffer(4, std::vector<ModT>(max_n)) {
    inv[1] = ModT(1);
    for (int i = 2; i < max_n; ++i) {
      inv[i] = ModT(ModT::MOD - ModT::MOD / i) * inv[ModT::MOD % i];
    }
  }

  void multiply(std::vector<ModT> &out, const std::vector<ModT> &f,
                const std::vector<ModT> &g) {
    ModT *b0 = buffer[0].data();
    ModT *b1 = buffer[1].data();

    int deg_plus_1 = f.size() + g.size() - 1;
    int n = min_power_of_two(deg_plus_1);
    assert_max_n(n);

    copy_and_fill(n, b0, f.size(), f.data());
    NTT::dif(n, b0);
    copy_and_fill(n, b1, g.size(), g.data());
    NTT::dif(n, b1);
    dot_product(n, b0, b0, b1);
    NTT::dit(n, b0);
    out.resize(deg_plus_1);
    for (int i = 0; i < deg_plus_1; ++i) {
      out[i] = b0[i];
    }
  }

  void inverse(int n, ModT *out, const ModT *f) {
    assert_power_of_two(n);
    assert_max_n(n);

    ModT *const b0 = buffer[0].data();
    ModT *const b1 = buffer[1].data();

    std::fill(out, out + n, ModT(0));
    out[0] = f[0].inverse();
    ModT inv_m(1);
    for (int m = 2; m <= n; m <<= 1) {
      copy_and_fill(m, b0, n, f);
      NTT::dif(m, b0);
      copy_and_fill(m, b1, m, out);
      NTT::dif(m, b1);
      inv_m *= ModT(2).inverse();
      dot_product(m, inv_m, b0, b0, b1);
      NTT::dit(m, b0);
      std::fill(b0, b0 + (m >> 1), ModT(0));
      NTT::dif(m, b0);
      dot_product(m, inv_m, b0, b0, b1);
      NTT::dit(m, b0);
      for (int i = m >> 1; i < m; ++i) {
        out[i] = ModT(0) - b0[i];
      }
    }
  }

  // f / g
  void divide(int n, ModT *out, const ModT *f, const ModT *g) {
    assert_power_of_two(n);
    assert_max_n(n);

    ModT *const b0 = buffer[0].data();
    ModT *const b1 = buffer[1].data();
    ModT *const inv_g = buffer[2].data();

    int m = n >> 1;
    inverse(m, inv_g, g);
    std::fill(inv_g + m, inv_g + n, ModT(0));
    NTT::dif(n, inv_g);
    copy_and_fill(n, b0, m, f);
    NTT::dif(n, b0);
    const ModT inv_n = ModT(n).inverse();
    dot_product(n, inv_n, b0, b0, inv_g);
    NTT::dit(n, b0);

    std::copy(b0, b0 + m, out);

    std::fill(b0 + m, b0 + n, ModT(0));
    NTT::dif(n, b0);
    copy_and_fill(n, b1, n, g);
    NTT::dif(n, b1);
    dot_product(n, inv_n, b0, b0, b1);
    NTT::dit(n, b0);
    std::fill(b0, b0 + m, ModT(0));
    for (int i = m; i < n; ++i) {
      b0[i] -= f[i];
    }
    NTT::dif(n, b0);
    dot_product(n, inv_n, b0, b0, inv_g);
    NTT::dit(n, b0);
    for (int i = m; i < n; ++i) {
      out[i] = ModT(0) - b0[i];
    }
  }

  void log(int n, ModT *out, const ModT *f) {
    assert_power_of_two(n);
    assert_max_n(n);
    // log f = \int f' / f
    ModT *const d_f = buffer[3].data();
    d_f[n - 1] = ModT(0);
    for (int i = 1; i < n; ++i) {
      d_f[i - 1] = ModT(i) * f[i];
    }
    divide(n, out, d_f, f);
    for (int i = n; i-- > 1;) {
      out[i] = out[i - 1] * inv[i];
    }
    out[0] = ModT(0);
  }

  // void exp(int n, ModT *out, const ModT *f) {}

private:
  static int min_power_of_two(int n) {
    return 1 << (32 - __builtin_clz(n - 1));
  }

  static void copy_and_fill(int n, ModT *dst, int m, const ModT *src) {
    m = std::min(n, m);
    std::copy(src, src + m, dst);
    std::fill(dst + m, dst + n, ModT(0));
  }

  static void dot_product(int n, ModT inv_n, ModT *out, const ModT *a,
                          const ModT *b) {
    for (int i = 0; i < n; ++i) {
      out[i] = inv_n * a[i] * b[i];
    }
  }

  static void dot_product(int n, ModT *out, const ModT *a, const ModT *b) {
    dot_product(n, ModT(n).inverse(), out, a, b);
  }

  void assert_max_n(int n) const {
    if (n > max_n) {
      throw std::invalid_argument("insufficient buffer");
    }
  }

  int max_n;
  std::vector<ModT> inv;
  std::vector<std::vector<ModT>> buffer;
};

} // namespace ntt
