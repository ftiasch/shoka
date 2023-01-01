#pragma once

#include <cstring>
#include <functional>
#include <vector>

template <typename T_, T_ id_, typename Compare = std::less<T_>>
struct SemilatticeT {
  using T = T_;

  static constexpr T id = id_;

  static constexpr T meet(const T &x, const T &y) {
    return Compare{}(x, y) ? x : y;
  }
};

template <typename Semilattice_> struct SparseTableT {
  using Semilattice = Semilattice_;
  using T = typename Semilattice::T;

  explicit SparseTableT(const std::vector<T> &value)
      : n{static_cast<int>(value.size())}, l(log2n(n)), log(n + 1),
        table(l, std::vector<T>(n)) {
    log[1] = 0;
    for (int i = 2; i <= n; ++i) {
      log[i] = log[i - 1] + ((1 << (log[i - 1] + 1)) < i);
    }
    table[0] = value;
    for (int i = 1; i < l; ++i) {
      for (int j = 0; j + (1 << i) <= n; ++j) {
        table[i][j] = Semilattice::meet(table[i - 1][j],
                                        table[i - 1][j + (1 << (i - 1))]);
      }
    }
  }

  // = meet(v[l], ..., v[r - 1])
  T operator()(int l, int r) const {
    if (l == r) {
      return Semilattice::id;
    }
    const int lv = log[r - l];
    return Semilattice::meet(table[lv][l], table[lv][r - (1 << lv)]);
  }

private:
  static constexpr int log2n(int n) {
    return n > 1 ? 32 - __builtin_clz(n - 1) : 1;
  }

  int n, l;
  std::vector<int> log;
  std::vector<std::vector<T>> table;
};
