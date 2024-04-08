#include "types/tree_monoid.h"

#include <utility>
#include <vector>

template <IsTreeMonoid M> struct RerootDp {
  using Tree = std::vector<std::vector<std::pair<int, M>>>;

  static std::vector<M> get_sum(const Tree &tree) {
    return RerootDp<M>{tree}.sum;
  }

  explicit RerootDp(const Tree &tree_)
      : tree{tree_}, n(tree.size()), up_edge(n), down(n), up(n), sum(n) {
    dfs_down(-1, 0);
    dfs_up(-1, 0);
  }

  void dfs_down(int p, int u) {
    for (auto &&[v, e] : tree[u]) {
      if (v != p) {
        dfs_down(u, v);
        down[v] = M::compress(e, down[v]);
        down[u] = M::rake(down[u], down[v]);
      } else {
        up_edge[u] = &e;
      }
    }
  }

  void dfs_up(int p, int u) {
    suffix.resize(tree[u].size() + 1);
    suffix[tree[u].size()] = M{};
    for (int i = tree[u].size(); i--;) {
      auto &&[v, _] = tree[u][i];
      suffix[i] = v == p ? suffix[i + 1] : M::rake(down[v], suffix[i + 1]);
    }
    M prefix{up[u]};
    for (int i = 0; i < tree[u].size(); i++) {
      auto &&[v, _] = tree[u][i];
      if (v != p) {
        up[v] = M::compress(*up_edge[v], M::rake(prefix, suffix[i + 1]));
        prefix = M::rake(prefix, down[v]);
      }
    }
    sum[u] = prefix;
    for (auto &&[v, _] : tree[u]) {
      if (v != p) {
        dfs_up(u, v);
      }
    }
  }

  const Tree &tree;
  int n;
  std::vector<const M *> up_edge;
  std::vector<M> down, up, sum, suffix;
};
