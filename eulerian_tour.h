#include "types/graph/adjacent_list_base.h"

#include <utility>
#include <vector>

class Eulerian : public AdjacentListBase<int> {
  void dfs(int u) {
    while (~head[u]) {
      int i = head[u];
      head[u] = next[head[u]];
      if (!used[i >> 1]) {
        used[i >> 1] = true;
        dfs(edges[i]);
      }
    }
    tour.push_back(u);
  }

  int n;
  std::vector<bool> used;

public:
  explicit Eulerian(int n, int s, const std::vector<std::pair<int, int>> &edges)
      : AdjacentListBase<int>{n, static_cast<int>(edges.size()) << 1}, n{n},
        used(edges.size() << 1) {
    for (auto [a, b] : edges) {
      add(a, b);
      add(b, a);
    }
    dfs(s);
  }

  std::vector<int> tour;
};
