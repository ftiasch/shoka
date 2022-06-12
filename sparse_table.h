#pragma once

#include <algorithm>
#include <cstring>
#include <vector>

template <typename T> struct SparseTable {
  SparseTable(const std::vector<T> &value)
      : n(value.size()), l(log2n(n)), log(n), table(l, std::vector<T>(n)) {
    log[0] = 0;
    for (int i = 1; i < n; ++i) {
      log[i] = log[i - 1] + (1 << (log[i - 1] + 1) < i + 1);
    }
    table[0] = value;
    for (int i = 1; i < l; ++i) {
      for (int j = 0; j + (1 << i) <= n; ++j) {
        table[i][j] =
            std::min(table[i - 1][j], table[i - 1][j + (1 << (i - 1))]);
      }
    }
  }

  T operator()(int l, int r) const {
    const int lv = log[r - l];
    return std::min(table[lv][l], table[lv][r - (1 << lv) + 1]);
  }

private:
  static constexpr int log2n(int n) {
    return n > 1 ? 32 - __builtin_clz(n - 1) : 1;
  }

  int n, l;
  std::vector<int> log;
  std::vector<std::vector<T>> table;
};
