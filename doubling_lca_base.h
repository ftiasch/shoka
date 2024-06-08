#pragma once

#include "snippets/min_pow_of_two.h"

#include <vector>

class DoublingLcaBase {
  int n, max_d;
  std::vector<int> depth;
  std::vector<std::vector<int>> jump;

public:
  explicit DoublingLcaBase(int n_)
      : n(n_), max_d{log_min_pow_of_two(n)}, depth(n),
        jump(n, std::vector<int>(max_d, -1)) {}

  void add_leaf(int u, int p) {
    depth[u] = depth[p] + 1;
    jump[u][0] = p;
    for (int i = 0; ~jump[u][i] && i + 1 < max_d; i++) {
      jump[u][i + 1] = jump[jump[u][i]][i];
    }
  }

  int lca(int x, int y) const {
    if (depth[x] > depth[y]) {
      std::swap(x, y);
    }
    for (int d = 0; d < max_d; d++) {
      if ((depth[y] - depth[x]) >> d & 1) {
        y = jump[y][d];
      }
    }
    if (x == y) {
      return x;
    }
    for (int d = max_d; d--;) {
      if (jump[x][d] != jump[y][d]) {
        x = jump[x][d];
        y = jump[y][d];
      }
    }
    return jump[x][0];
  }
};
