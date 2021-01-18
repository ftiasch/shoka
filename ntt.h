#include <algorithm>
#include <cstdint>
#include <vector>

using u32 = uint32_t;
using u64 = uint64_t;


template <typename ModT> struct NTT {
  static const u32 MOD = ModT::MOD;

  NTT(int n) : n(n), dit_twiddles(n), dif_twiddles(n) { resize(n); }

  void resize(int n) {
    this->n = n;
    inv_n = power(ModT(n), MOD - 2);
    dit_twiddles.resize(n);
    dif_twiddles.resize(n);
    ModT root = power(G, (MOD - 1) / n);
    for (int m = n; m >>= 1;) {
      dit_twiddles[m] = ModT(1);
      for (int i = 1; i < m; ++i) {
        dit_twiddles[m + i] = dit_twiddles[m + i - 1] * root;
      }
      root *= root;
    }
    root = power(G, MOD - 1 - (MOD - 1) / n);
    for (int m = n; m >>= 1;) {
      dif_twiddles[m] = ModT(1);
      for (int i = 1; i < m; ++i) {
        dif_twiddles[m + i] = dif_twiddles[m + i - 1] * root;
      }
      root *= root;
    }
  }

  void dit(ModT *a) {
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

  void dif(ModT *a) {
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

  void convolute(ModT *a, ModT *b, ModT *out) {
    dif(a);
    dif(b);
    for (int i = 0; i < n; ++i) {
      out[i] = inv_n * a[i] * b[i];
    }
    dit(out);
  }

  void revbin(ModT *a) {
    for (int i = 1, j = 0; i < n - 1; ++i) {
      for (int s = n; j ^= s >>= 1, ~j & s;)
        ;
      if (i < j) {
        std::swap(a[i], a[j]);
      }
    }
  }

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
