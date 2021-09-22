#include "heavy_light_decomp_base.h"

template <typename NestedT, bool BIASED = false>
struct HeavyLightDecompositionSubtree : public HeavyLightDecompositionBase {
  HeavyLightDecompositionSubtree(const Tree &tree, int root)
      : HeavyLightDecompositionBase(tree, root), path(n, nullptr) {
    if (BIASED) {
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
    } else {
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
  }

  ~HeavyLightDecompositionSubtree() {
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
