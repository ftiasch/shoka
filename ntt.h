namespace ntt {
template <int MOD> struct FiniteField {
  static constexpr int primitive_root() {
    int g = 2;
    while (!is_primitive_root(g)) {
      g++;
    }
    return g;
  }

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
} // namespace ntt
