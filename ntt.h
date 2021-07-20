#include "mod.h"

#include <algorithm>
#include <cassert>
#include <cstdint>
#include <stdexcept>
#include <vector>

namespace ntt {

using u32 = uint32_t;
using u64 = uint64_t;

static void assert_power_of_two(int n) {
  if (n & (n - 1)) {
    throw std::invalid_argument(std::to_string(n) + " is not a power of two");
  }
}

template <typename ModT_> struct NTT {
  using ModT = ModT_;
  static const u32 MOD = ModT::MOD;

  NTT(int n) : n(n), dit_twiddles(n), dif_twiddles(n) { resize(n); }

  void resize(int n) {
    assert_power_of_two(n);
    this->n = n;
    inv_n = ModT(n).inverse();
    dit_twiddles.resize(n);
    dif_twiddles.resize(n);
    ModT root = G.power((MOD - 1) / n);
    for (int m = n; m >>= 1;) {
      dit_twiddles[m] = ModT{1};
      for (int i = 1; i < m; ++i) {
        dit_twiddles[m + i] = dit_twiddles[m + i - 1] * root;
      }
      root *= root;
    }
    root = G.power((MOD - 1) / n).inverse();
    for (int m = n; m >>= 1;) {
      dif_twiddles[m] = ModT{1};
      for (int i = 1; i < m; ++i) {
        dif_twiddles[m + i] = dif_twiddles[m + i - 1] * root;
      }
      root *= root;
    }
  }

  void dit(ModT *a) const {
    for (int m = 1; m < n; m <<= 1) {
      for (int i = 0; i < n; i += m << 1) {
        for (int r = i; r < i + m; ++r) {
          ModT tmp = dit_twiddles[m + r - i] * a[r + m];
          a[r + m] = a[r];
          a[r + m] -= tmp;
          a[r] += tmp;
        }
      }
    }
  }

  void dif(ModT *a) const {
    for (int m = n; m >>= 1;) {
      for (int i = 0; i < n; i += m << 1) {
        for (int r = i; r < i + m; ++r) {
          ModT tmp = a[r];
          tmp -= a[r + m];
          a[r] += a[r + m];
          a[r + m] = dif_twiddles[m + r - i] * tmp;
        }
      }
    }
  }

  void convolute(ModT *a, ModT *b, ModT *out) const {
    dif(a);
    dif(b);
    for (int i = 0; i < n; ++i) {
      out[i] = inv_n * a[i] * b[i];
    }
    dit(out);
  }

  static void dit(int n, ModT *a) {
    assert_power_of_two(n);
    for (int m = 1; m < n; m <<= 1) {
      const ModT root = G.power((MOD - 1) / (m << 1));
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
      const ModT root = power(G, MOD - 1 - (MOD - 1) / (m << 1));
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

  static void convolute(int n, ModT *a, ModT *b, ModT *out) {
    dif(n, a);
    dif(n, b);
    const ModT inv_n = ModT(n).inverse();
    for (int i = 0; i < n; ++i) {
      out[i] = inv_n * a[i] * b[i];
    }
    dit(n, out);
  }

  // void revbin(ModT *a) {
  //   for (int i = 1, j = 0; i < n - 1; ++i) {
  //     for (int s = n; j ^= s >>= 1, ~j & s;)
  //       ;
  //     if (i < j) {
  //       std::swap(a[i], a[j]);
  //     }
  //   }
  // }

private:
  static constexpr ModT power(ModT a, int n) {
    ModT res(1);
    while (n) {
      if (n & 1) {
        res *= a;
      }
      a *= a;
      n >>= 1;
    }
    return res;
  }

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
      for (int d = 2; d * d <= MOD - 1; ++d) {
        if ((MOD - 1) % d == 0 &&
            (power(g, d).get() == 1 || power(g, (MOD - 1) / d).get() == 1)) {
          return false;
        }
      }
      return true;
    }
  };

  int n;
  ModT inv_n;
  std::vector<ModT> dit_twiddles, dif_twiddles;

  static constexpr ModT G = ModT(FiniteField::primitive_root());
};

template <typename T> struct Span {
  Span(T *data, size_t size) : data_{data}, size_{size} {}

  T *const data() const { return data_; }

  size_t size() const { return size_; }

private:
  T *data_;
  size_t size_;
};

template <typename NTT>
static void inverse(Span<typename NTT::ModT> buffer, int n,
                    const typename NTT::ModT *p, typename NTT::ModT *q) {
  using ModT = typename NTT::ModT;
  if (static_cast<int>(buffer.size()) < 2 * n) {
    throw std::invalid_argument("insufficient buffer");
  }
  assert_power_of_two(n);
  if (p[0].get() == 0) {
    throw std::invalid_argument("coefficient[0] == 0");
  }
  std::fill(q, q + n, ModT{0});
  q[0] = p[0].inverse();
  ModT *const dif_q = buffer.data();
  ModT *const dif_p = buffer.data() + n;
  ModT inv_2m = ModT{2}.inverse();
  for (int m = 1; m < n; m <<= 1) {
    const int _2m = m << 1;
    std::copy(q, q + _2m, dif_q);
    NTT::dif(_2m, dif_q);
    std::copy(p, p + _2m, dif_p);
    NTT::dif(_2m, dif_p);
    for (int i = 0; i < _2m; ++i) {
      dif_p[i] = inv_2m * dif_p[i] * dif_q[i];
    }
    NTT::dit(_2m, dif_p);
    std::fill(dif_p, dif_p + m, ModT{0});
    NTT::dif(_2m, dif_p);
    for (int i = 0; i < _2m; ++i) {
      dif_p[i] = inv_2m * dif_p[i] * dif_q[i];
    }
    NTT::dit(_2m, dif_p);
    for (int i = m; i < _2m; ++i) {
      q[i] -= dif_p[i];
    }
    inv_2m *= ModT{2}.inverse();
  }
}

template <typename NTT>
static void differentiate(int n, const typename NTT::ModT *p,
                          typename NTT::ModT *dp) {
  using ModT = typename NTT::ModT;
  for (int i = 0; i < n; ++i) {
    dp[i] = i + 1 < n ? ModT{i + 1} * p[i + 1] : ModT{0};
  }
}

template <typename NTT>
static void integrate(Span<typename NTT::ModT> buffer, int n,
                      const typename NTT::ModT *p, typename NTT::ModT *dp) {
  using ModT = typename NTT::ModT;
  if (p[n - 1] != ModT{0}) {
    throw std::invalid_argument("p[n - 1] != 0");
  }
  ModT *const inv = buffer.data();
  for (int i = 1; i < n; ++i) {
    inv[i] =
        i == 1 ? ModT{1} : ModT(ModT::MOD - ModT::MOD / i) * inv[ModT::MOD % i];
  }
  for (int i = n; i-- > 1;) {
    dp[i] = inv[i] * p[i - 1];
  }
  dp[0] = ModT{0};
}

template <typename NTT> struct Inverse {
  using ModT = typename NTT::ModT;

  Inverse(int max_n) : buffer(max_n << 1) {}

  void operator()(int n, const ModT *p, ModT *q) {
    return inverse<NTT>(Span<ModT>{buffer.data(), buffer.size()}, n, p, q);
  }

private:
  std::vector<ModT> buffer;
};

// TODO
// template <typename NTT> struct Logarithm {
//   using ModT = typename NTT::ModT;

//   Logarithm() : buffer(max_n << 2) {}

//   // log p = int{p' / p}
//   void operator()(int n, const ModT *p, ModT *out) {
//     ModT *const inv_p = buffer.data();
//     ModT *const dp = buffer.data() + (n << 1);
//     inverse<NTT>(Span<ModT>{buffer.data(), 2 * n}, n, p, inv_p);
//     differentiate<NTT>(n, p, dp);
//   }

// private:
//   const int max_n;
//   std::vector<ModT> buffer;
// };

} // namespace ntt
