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
  Poly(int max_n_) : max_n(max_n_), buffer(4, std::vector<ModT>(max_n)) {}

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

    ModT *b0 = buffer[0].data();
    ModT *b1 = buffer[1].data();

    out[0] = f[0].inverse();
    ModT inv_2m(1);
    for (int m = 1; m < n; m <<= 1) {
      copy_and_fill(m << 1, b0, n, f);
      NTT::dif(m << 1, b0);
      copy_and_fill(m << 1, b1, m, out);
      NTT::dif(m << 1, b1);
      inv_2m *= ModT(2).inverse();
      dot_product(m << 1, inv_2m, b0, b0, b1);
      NTT::dit(m << 1, b0);
      std::fill(b0, b0 + m, ModT(0));
      NTT::dif(m << 1, b0);
      dot_product(m << 1, inv_2m, b0, b0, b1);
      NTT::dit(m << 1, b0);
      for (int i = m; i < m << 1; ++i) {
        out[i] = ModT(0) - b0[i];
      }
    }
  }

  // g / f
  // void divide(int n, std::vector<ModT>& out, const std::vector<ModT>& g,
  // const std::vector<ModT>& f) {
  //   ModT *b0 = buffer[0].data();
  //   ModT *b1 = buffer[1].data();

  //   int m = get_n(n) >> 1;
  //   inverse(m, out, f);
  //   out.resize(m << 1);
  //   NTT::dif(m << 1, out.data());
  //   assign(m << 1, b0, g);
  //   NTT::dif(m << 1, b0);
  //   const ModT inv_2m = ModT(m << 1).inverse();
  //   for (int i = 0; i < m << 1; ++ i) {
  //     b0[i] = inv_2m * b0[i] * out[i];
  //   }
  //   NTT::dit(m << 1, b0);
  // }

  // void log(int n, ModT *out, const ModT *f) {}

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
  std::vector<std::vector<ModT>> buffer;
};

} // namespace ntt
