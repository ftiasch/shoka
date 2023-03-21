#include <algorithm>
#include <vector>

struct HeavyLightDecompositionBase {
  using Tree = std::vector<std::vector<int>>;

  explicit HeavyLightDecompositionBase(const Tree &tree, int root)
      : n(tree.size()), parent(n), size(n), depth(n), top(n), bottom(n) {
    parent[root] = -1;
    build(tree, root);
    for (int u = 0; u < n; ++u) {
      top[u] = top[bottom[u]];
    }
  }

  int lca(int a, int b) const {
    while (bottom[a] != bottom[b]) {
      if (get_lowest_depth(a) > get_lowest_depth(b)) {
        a = top[a];
      } else {
        b = top[b];
      }
    }
    return depth[a] < depth[b] ? a : b;
  }

  int n;
  std::vector<int> parent, size, depth;

protected:
  void build(const Tree &tree, int u) {
    const int p = parent[u];
    depth[u] = ~p ? depth[p] + 1 : 0;
    size[u] = 1;
    std::pair<int, int> candidate{0, u};
    for (int v : tree[u]) {
      if (v != p) {
        parent[v] = u;
        build(tree, v);
        size[u] += size[v];
        candidate = std::max(candidate, std::make_pair(size[v], bottom[v]));
      }
    }
    bottom[u] = candidate.second;
    top[bottom[u]] = p;
  }

  int get_lowest_depth(int u) const { return ~top[u] ? depth[top[u]] : -1; }

  std::vector<int> top, bottom;
};
