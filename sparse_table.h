#include <algorithm>
#include <cstring>

template <typename T, int N> struct SparseTable {
  SparseTable() {
    log[1] = 0;
    for (int i = 2; i <= N; ++i) {
      log[i] = 31 - __builtin_clz(i - 1);
    }
  }

  void compute(int n, const T *value) {
    l = log2n(n), memcpy(st[0], value, sizeof(T) * n);
    for (int i = 1; i < l; ++i) {
      for (int j = 0; j + (1 << i) <= n; ++j) {
        st[i][j] = std::min(st[i - 1][j], st[i - 1][j + (1 << (i - 1))]);
      }
    }
  }

  T rmq(int a, int b) const {
    const int l = log[b - a];
    return std::min(st[l][a], st[l][b - (1 << l)]);
  }

private:
  static constexpr int log2n(int n) { return 32 - __builtin_clz(n - 1); }

  static const int L = log2n(N);

  int l, log[N + 1];
  T st[L][N];
};
