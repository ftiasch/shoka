#include <algorithm>
#include <vector>

template <typename NestedT> struct HeavyLightDecomposition {
  using Tree = std::vector<std::vector<int>>;

  HeavyLightDecomposition(const Tree &tree, int root)
      : n(tree.size()), parent(n), size(n), depth(n), path(n, nullptr),
        lowest(n), highest(n) {
    parent[root] = -1;
    build(tree, root);
    for (int u = 0; u < n; ++u) {
      lowest[u] = lowest[highest[u]];
    }
  }

  void init_biased_nested() {
    for (int u = 0; u < n; ++u) {
      if (highest[u] == u) {
        const int count = depth[u] - get_lowest_depth(u);
        std::vector<int> vertices, weight;
        vertices.reserve(count);
        weight.reserve(count);
        for (int i = 0, v = u; i < count; ++i, v = parent[v]) {
          vertices.push_back(v);
          weight.push_back(size[v]);
        }
        for (int i = count; i-- > 1;) {
          weight[i] -= weight[i - 1];
        }
        path[u] = new NestedT(vertices, weight);
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
        h.template update<0>(path[highest[a]], base - depth[a],
                             base - get_lowest_depth(a) - 1);
        a = lowest[a];
      } else {
        const int base = depth[highest[b]];
        h.template update<1>(path[highest[b]], base - depth[b],
                             base - get_lowest_depth(b) - 1);
        b = lowest[b];
      }
    }
    if (include_lca || a != b) {
      if (depth[a] > depth[b]) {
        const int base = depth[highest[a]];
        h.template update<0>(path[highest[a]], base - depth[a],
                             base - depth[b] - (!include_lca));
      } else {
        const int base = depth[highest[b]];
        h.template update<1>(path[highest[b]], base - depth[b],
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
    return depth[a] < depth[b] ? a : b;
  }

  int n;
  std::vector<int> parent, size, depth;

private:
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
        candidate = std::max(candidate, std::make_pair(size[v], highest[v]));
      }
    }
    highest[u] = candidate.second;
    lowest[highest[u]] = p;
  }

  int get_lowest_depth(int u) const {
    return ~lowest[u] ? depth[lowest[u]] : -1;
  }

  std::vector<NestedT *> path;
  std::vector<int> lowest, highest;
};
