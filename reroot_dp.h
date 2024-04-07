#include "types/tree_monoid.h"

#include <utility>
#include <vector>

template <IsTreeMonoid M> struct RerootDp {
  using Tree = std::vector<std::vector<std::pair<int, M>>>;
  using VM = std::vector<M>;

  explicit RerootDp(const Tree &tree_)
      : tree{tree_}, n(tree.size()), down(n), up(n) {
    VM up_edge(n), down1(n);
    dfs_down(up_edge, down1, -1, 0);
    VM suffix;
    dfs_up(up_edge, down1, suffix, -1, 0);
  }

  M all(int u) const { return M::rake(down[u], up[u]); }

  void dfs_down(VM &up_edge, VM &down1, int p, int u) {
    for (auto &&[v, e] : tree[u]) {
      if (v != p) {
        dfs_down(up_edge, down1, u, v);
        down1[v] = M::compress(e, down[v]);
        down[u] = M::rake(down[u], down1[v]);
      } else {
        up_edge[u] = e;
      }
    }
  }

  void dfs_up(VM &up_edge, VM &down1, VM &suffix, int p, int u) {
    suffix.resize(tree[u].size() + 1);
    suffix[tree[u].size()] = M{};
    for (int i = tree[u].size(); i--;) {
      auto &&[v, _] = tree[u][i];
      suffix[i] = v == p ? suffix[i + 1] : M::rake(down1[v], suffix[i + 1]);
    }
    M prefix{up[u]};
    for (int i = 0; i < tree[u].size(); i++) {
      auto &&[v, _] = tree[u][i];
      if (v != p) {
        up[v] = M::compress(up_edge[v], M::rake(prefix, suffix[i + 1]));
        prefix = M::rake(prefix, down1[v]);
      }
    }
    for (auto &&[v, _] : tree[u]) {
      if (v != p) {
        dfs_up(up_edge, down1, suffix, u, v);
      }
    }
  }

  const Tree &tree;
  int n;
  std::vector<M> down, up;
};
