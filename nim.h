namespace nim {
using u64 = uint64_t;

template <int k, typename Out> struct Reducer {
  static_assert(k <= 6, "");

  u64 multiply(u64 a, u64 b) const {
    static const u64 P = 1ULL << (1 << k - 1);
    u64 a1 = a / P, a0 = a % P, b1 = b / P, b0 = b % P,
        c = out.multiply(a0, b0);
    return out.multiply(out.multiply(a1, b1), P / 2) ^
           (out.multiply(a0 ^ a1, b0 ^ b1) ^ c) * P ^ c;
  }

private:
  Out out;
};

template <int k> struct QuadraticMemorizedNim {
  static_assert(k <= 3, "");

  static const int N = 1 << (1 << k);

  QuadraticMemorizedNim() : table() {
    static Reducer<k, QuadraticMemorizedNim<k - 1>> out;
    for (int i = 0; i < N; ++i) {
      for (int j = 0; j < N; ++j) {
        table[i][j] = out.multiply(i, j);
      }
    }
  }

  u64 multiply(u64 a, u64 b) const { return table[a][b]; }

  u64 table[N][N];
};

template <> struct QuadraticMemorizedNim<0> {
  static const int N = 2;

  QuadraticMemorizedNim() {}

  u64 multiply(u64 a, u64 b) const { return a && b; }
};

// k == 4
template <u64 G> struct LinearMemorizedNim {
  static const int N = 1 << 16;

  LinearMemorizedNim() : exp(), log() {
    static Reducer<4, Reducer<3, QuadraticMemorizedNim<3>>> reduced;
    log[exp[0] = 1] = 0;
    for (int i = 1; i < N - 1; ++i) {
      log[exp[i] = reduced.multiply(exp[i - 1], G)] = i;
    }
  }

  u64 multiply(u64 a, u64 b) const {
    return a == 0 || b == 0 ? 0 : exp[(log[a] + log[b]) % (N - 1)];
  }

private:
  int exp[N], log[N];
};

u64 multiply(u64 a, u64 b) {
  static Reducer<6, Reducer<5, Reducer<4, LinearMemorizedNim<258>>>> reduced;
  return reduced.multiply(a, b);
}
} // namespace nim