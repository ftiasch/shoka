template <typename NestedT> struct HeavyLightDecomposition {
  using Tree = std::vector<std::vector<int>>;

  explicit HeavyLightDecomposition(int n, const Tree &tree, int root)
      : path(n), depth(n), lowest(n), highest(n) {
    build(tree, -1, root);
    for (int u = 0; u < n; ++u) {
      lowest[u] = lowest[highest[u]];
      if (highest[u] == u) {
        path[u] = new NestedT{depth[u] - get_lowest_depth(u)};
      }
    }
  }

  template <typename Result> Result query(bool include_lca, int a, int b) {
    Result result;
    while (highest[a] != highest[b]) {
      int &p = get_lowest_depth(a) > get_lowest_depth(b) ? a : b;
      int base = depth[highest[p]];
      result.update(path[highest[p]], base - depth[p],
                    base - get_lowest_depth(p) - 1);
      p = lowest[p];
    }
    if (include_lca || a != b) {
      if (depth[a] < depth[b]) {
        std::swap(a, b);
      }
      // depth[a] >= depth[b]
      int base = depth[highest[a]];
      result.update(path[highest[a]], base - depth[a],
                    base - depth[b] - (!include_lca));
    }
    return result;
  }

private:
  int build(const Tree &tree, int p, int u) {
    depth[u] = ~p ? depth[p] + 1 : 0;
    int size = 1;
    std::pair<int, int> candidate{0, u};
    for (int v : tree[u]) {
      if (v != p) {
        int subtree_size = build(tree, u, v);
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
  std::vector<int> depth, lowest, highest;
};
