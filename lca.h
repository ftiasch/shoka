#pragma once

#include "snippets/min_pow_of_two.h"
#include "types/graph.h"

#include <vector>

template <IsGraph Graph>
  requires std::is_same_v<GraphEdge<Graph>, int>
class Lca {
  void dfs(int p, int u) {
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

  const Graph &tree;
  int n, max_d;
  std::vector<int> depth;
  std::vector<std::vector<int>> jump;

public:
  explicit Lca(const Graph &tree_, int root)
      : tree{tree_}, n(tree.size()), max_d{min_pow_of_two(n)}, depth(n),
        jump(n, std::vector<int>(max_d, -1)) {
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
};
