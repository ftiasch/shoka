#pragma once

#include "doubling_lca_base.h"
#include "types/graph.h"

template <IsGraph Graph>
  requires std::is_same_v<GraphEdge<Graph>, int>
class Lca : public DoublingLcaBase {
  void dfs(int p, int u) {
    if (~p) {
      add_leaf(u, p);
    }
    for (auto &&v : tree[u]) {
      if (v != p) {
        dfs(u, v);
      }
    }
  };

  const Graph &tree;

public:
  explicit Lca(const Graph &tree_, int root)
      : DoublingLcaBase(tree_.size()), tree{tree_} {
    dfs(-1, root);
  }

  int operator()(int a, int b) { return lca(a, b); }
};
