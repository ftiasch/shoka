#pragma once

#include <functional>
#include <vector>

struct Solver {
  explicit Solver(const std::vector<std::vector<int>> &tree, int root)
      : n(tree.size()), max_d{get_max_d(n)}, depth(n),
        jump(n, std::vector<int>(max_d, -1)) {
    std::function<void(int, int)> dfs = [&](int p, int u) -> void {
      depth[u] = ~p ? depth[p] + 1 : 0;
      jump[u][0] = p;
      for (int i = 0; ~jump[u][i] && i + 1 < max_d; i++) {
        jump[u][i + 1] = jump[jump[u][i]][i];
      }
      for (auto &&v : tree[u]) {
        if (v != p) {
          dfs(u, v);
        }
      }
    };
    dfs(-1, root);
  }

  int operator()(int x, int y) const {
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

  static constexpr int get_max_d(int n) {
    int d = 1;
    while ((1 << d) < n) {
      d++;
    }
    return d;
  }

  int n, max_d;
  std::vector<int> depth;
  std::vector<std::vector<int>> jump;
};
