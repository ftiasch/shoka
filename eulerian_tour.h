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
  explicit Eulerian(int n_, int s,
                    const std::vector<std::pair<int, int>> &edges_)
      : AdjacentListBase<int>{n_, static_cast<int>(edges_.size()) << 1}, n{n_},
        used(edges_.size() << 1) {
    for (auto [a, b] : edges_) {
      add(a, b);
      add(b, a);
    }
    dfs(s);
  }

  std::vector<int> tour;
};
