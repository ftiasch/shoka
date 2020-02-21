template <int N, int MOD> struct NTT {
  void transform(int n, int *a) {
    for (int m = 1; m < n; m <<= 1) {
      int64_t root = power(G, (MOD - 1) / (m << 1));
      twiddles[0] = 1;
      for (int i = 1; i < m; ++i) {
        twiddles[i] = twiddles[i - 1] * root % MOD;
      }
      for (int i = 0; i < n; i += m << 1) {
        for (int r = i; r < i + m; ++r) {
          int tmp = static_cast<int64_t>(twiddles[r - i]) * a[r + m] % MOD;
          a[r + m] = a[r];
          add(a[r + m], MOD - tmp);
          add(a[r], tmp);
        }
      }
    }
  }

  void itransform(int n, int *a) {
    for (int m = n; m >>= 1;) {
      int64_t root = power(G, MOD - 1 - (MOD - 1) / (m << 1));
      twiddles[0] = 1;
      for (int i = 1; i < m; ++i) {
        twiddles[i] = twiddles[i - 1] * root % MOD;
      }
      for (int i = 0; i < n; i += m << 1) {
        for (int r = i; r < i + m; ++r) {
          int tmp = a[r];
          add(tmp, MOD - a[r + m]);
          add(a[r], a[r + m]);
          a[r + m] = static_cast<int64_t>(twiddles[r - i]) * tmp % MOD;
        }
      }
    }
  }

  void convolute(int n, int *a, int *b, int *out) {
    itransform(n, a);
    itransform(n, b);
    int64_t inv_n = power(n, MOD - 2);
    for (int i = 0; i < n; ++i) {
      out[i] = inv_n * a[i] % MOD * b[i] % MOD;
    }
    transform(n, out);
  }

private:
  static constexpr int power(int a, int n) {
    int res = 1;
    while (n) {
      if (n & 1) {
        res = 1ULL * res * a % MOD;
      }
      a = 1ULL * a * a % MOD;
      n >>= 1;
    }
    return res;
  }

  static void add(int &x, int a) {
    x += a;
    if (x >= MOD) {
      x -= MOD;
    }
  }

  struct FiniteField {
    static constexpr int primitive_root() {
      int g = 2;
      while (!is_primitive_root(g)) {
        g++;
      }
      return g;
    }

  private:
    static constexpr bool is_primitive_root(int g) {
      for (int d = 2; d * d <= MOD - 1; ++d) {
        if ((MOD - 1) % d == 0 &&
            (power(g, d) == 1 || power(g, (MOD - 1) / d) == 1)) {
          return false;
        }
      }
      return true;
    }
  };

  static const int G = FiniteField::primitive_root();
  int twiddles[N];
};