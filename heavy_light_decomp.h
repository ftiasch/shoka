#include <algorithm>
#include <vector>

struct HeavyLightDecompositionBase {
  using Tree = std::vector<std::vector<int>>;

  HeavyLightDecompositionBase(const Tree &tree, int root)
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

template <typename NestedT>
struct HeavyLightDecomposition : public HeavyLightDecompositionBase {
  HeavyLightDecomposition(const Tree &tree, int root)
      : HeavyLightDecompositionBase(tree, root), path(n, nullptr) {}

  void init() {
    for (int u = 0; u < n; ++u) {
      if (bottom[u] == u) {
        const int count = depth[u] - get_lowest_depth(u);
        std::vector<int> vertices, weight;
        vertices.reserve(count);
        for (int i = 0, v = u; i < count; ++i, v = parent[v]) {
          vertices.push_back(v);
        }
        path[u] = new NestedT(vertices);
      }
    }
  }

  void init_biased() {
    for (int u = 0; u < n; ++u) {
      if (bottom[u] == u) {
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
    while (bottom[a] != bottom[b]) {
      if (get_lowest_depth(a) > get_lowest_depth(b)) {
        const int base = depth[bottom[a]];
        h.template update<0>(path[bottom[a]], base - depth[a],
                             base - get_lowest_depth(a) - 1);
        a = top[a];
      } else {
        const int base = depth[bottom[b]];
        h.template update<1>(path[bottom[b]], base - depth[b],
                             base - get_lowest_depth(b) - 1);
        b = top[b];
      }
    }
    if (include_lca || a != b) {
      if (depth[a] > depth[b]) {
        const int base = depth[bottom[a]];
        h.template update<0>(path[bottom[a]], base - depth[a],
                             base - depth[b] - (!include_lca));
      } else {
        const int base = depth[bottom[b]];
        h.template update<1>(path[bottom[b]], base - depth[b],
                             base - depth[a] - (!include_lca));
      }
    }
    return h;
  }

private:
  std::vector<NestedT *> path;
};
