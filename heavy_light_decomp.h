#include <algorithm>
#include <vector>

template <typename NestedT> struct HeavyLightDecomposition {
  using Tree = std::vector<std::vector<int>>;

  explicit HeavyLightDecomposition(int n, const Tree &tree, int root)
      : path(n, nullptr), parent(n), depth(n), lowest(n), highest(n) {
    parent[root] = -1;
    build(tree, root);
    for (int u = 0; u < n; ++u) {
      lowest[u] = lowest[highest[u]];
      if (highest[u] == u) {
        const int size = depth[u] - get_lowest_depth(u);
        std::vector<int> vertices;
        vertices.reserve(size);
        for (int i = 0, v = u; i < size; ++i, v = parent[v]) {
          vertices.push_back(v);
        }
        path[u] = new NestedT{vertices};
      }
    }
  }

  ~HeavyLightDecomposition() {
    for (auto &&p : path) {
      if (p != nullptr) {
        delete p;
      }
    }
  }

  template <typename Handler>
  Handler traverse(Handler &&h, bool include_lca, int a, int b) {
    while (highest[a] != highest[b]) {
      if (get_lowest_depth(a) > get_lowest_depth(b)) {
        const int base = depth[highest[a]];
        h.update<0>(path[highest[a]], base - depth[a],
                    base - get_lowest_depth(a) - 1);
        a = lowest[a];
      } else {
        const int base = depth[highest[b]];
        h.update<1>(path[highest[b]], base - depth[b],
                    base - get_lowest_depth(b) - 1);
        b = lowest[b];
      }
    }
    if (include_lca || a != b) {
      if (depth[a] > depth[b]) {
        const int base = depth[highest[a]];
        h.update<0>(path[highest[a]], base - depth[a],
                    base - depth[b] - (!include_lca));
      } else {
        const int base = depth[highest[b]];
        h.update<1>(path[highest[b]], base - depth[b],
                    base - depth[a] - (!include_lca));
      }
    }
    return h;
  }

  int lca(int a, int b) const {
    while (highest[a] != highest[b]) {
      if (get_lowest_depth(a) > get_lowest_depth(b)) {
        a = lowest[a];
      } else {
        b = lowest[b];
      }
    }
    return depth[a] > depth[b] ? b : a;
  }

private:
  int build(const Tree &tree, int u) {
    const int p = parent[u];
    depth[u] = ~p ? depth[p] + 1 : 0;
    int size = 1;
    std::pair<int, int> candidate{0, u};
    for (int v : tree[u]) {
      if (v != p) {
        parent[v] = u;
        int subtree_size = build(tree, v);
        size += subtree_size;
        candidate =
            std::max(candidate, std::make_pair(subtree_size, highest[v]));
      }
    }
    highest[u] = candidate.second;
    lowest[highest[u]] = p;
    return size;
  }

  int get_lowest_depth(int u) const {
    return ~lowest[u] ? depth[lowest[u]] : -1;
  }

  std::vector<NestedT *> path;
  std::vector<int> parent, depth, lowest, highest;
};
