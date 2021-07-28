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

    int n = f.size() + g.size() - 1;
    int n2 = get_n(n);
    assign(n2, b0, f);
    NTT::dif(n2, b0);
    assign(n2, b1, g);
    NTT::dif(n2, b1);
    const ModT inv_n = ModT(n2).inverse();
    for (int i = 0; i < n2; ++i) {
      b0[i] = inv_n * b0[i] * b1[i];
    }
    NTT::dit(n2, b0);
    out.resize(n);
    for (int i = 0; i < n; ++i) {
      out[i] = b0[i];
    }
  }

  // void inverse(int n, ModT *out, const ModT *f) {}

  // void divide(int n, ModT *out, const ModT *f, const ModT *g) {}

  // void log(int n, ModT *out, const ModT *f) {}

  // void exp(int n, ModT *out, const ModT *f) {}

private:
  static void assign(int n, ModT *buffer, const std::vector<ModT> &f) {
    std::copy(f.begin(), f.end(), buffer);
    std::fill(buffer + f.size(), buffer + n, ModT(0));
  }

  int get_n(int n) const {
    int n2 = 1;
    while (n2 < n) {
      n2 <<= 1;
    }
    if (n2 > max_n) {
      throw std::invalid_argument("insufficient buffer");
    }
    return n2;
  }

  int max_n;
  std::vector<std::vector<ModT>> buffer;
};

} // namespace ntt
