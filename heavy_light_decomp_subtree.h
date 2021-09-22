#include "heavy_light_decomp_base.h"

template <typename NestedT, bool BIASED = false>
struct HeavyLightDecompositionSubtree : public HeavyLightDecompositionBase {
  HeavyLightDecompositionSubtree(const Tree &tree, int root)
      : HeavyLightDecompositionBase(tree, root), position(n) {
    vertices.reserve(n);
    dfs(tree, root);
    sgt = new NestedT(vertices);
  }

  ~HeavyLightDecompositionSubtree() { delete sgt; }

  template <typename Handler> Handler traverse_all(Handler &&h) {
    return sgt->template traverse_all<Handler>(std::forward<Handler>(h));
  }

  template <typename Handler> Handler traverse_subtree(Handler &&h, int u) {
    return sgt->template traverse<Handler>(
        std::forward<Handler>(h), position[u], position[u] + size[u] - 1);
  }

  template <typename Handler>
  Handler traverse_path(Handler &&h, bool include_lca, int a, int b) {
    while (bottom[a] != bottom[b]) {
      if (get_lowest_depth(a) > get_lowest_depth(b)) {
        h.template update<0>(sgt,
                             position[a] - (depth[a] - get_lowest_depth(a)) + 1,
                             position[a]);
        a = top[a];
      } else {
        h.template update<1>(sgt,
                             position[b] - (depth[b] - get_lowest_depth(b)) + 1,
                             position[b]);
        b = top[b];
      }
    }
    if (include_lca || a != b) {
      if (depth[a] > depth[b]) {
        h.template update<0>(sgt, position[b] + (!include_lca), position[a]);
      } else {
        h.template update<1>(sgt, position[a] + (!include_lca), position[b]);
      }
    }
    return h;
  }

private:
  void dfs(const Tree &tree, int u) {
    position[u] = vertices.size();
    vertices.push_back(u);
    for (int v : tree[u]) {
      if (v != parent[u] && bottom[v] == bottom[u]) {
        dfs(tree, v);
        break;
      }
    }
    for (int v : tree[u]) {
      if (v != parent[u] && bottom[v] != bottom[u]) {
        dfs(tree, v);
      }
    }
  }

  std::vector<int> vertices, position;
  NestedT *sgt;
};
