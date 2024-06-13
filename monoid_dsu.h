#include "types/monoid.h"

#include <vector>

template <IsMonoid M> class MonoidDsu {
  std::vector<std::pair<int, M>> parent;

public:
  explicit MonoidDsu(int n) {
    parent.reserve(n);
    for (int i = 0; i < n; i++) {
      parent.emplace_back(i, M{});
    }
  }

  std::pair<int, M> find(int u) {
    if (parent[u].first != u) {
      auto [r, pm] = find(parent[u].first);
      parent[u] = {r, M::plus(pm, parent[u].second)};
    }
    return parent[u];
  }

  void link(int u, int v, const M &w) { parent[u] = {v, w}; }
};
