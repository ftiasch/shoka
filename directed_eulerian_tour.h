#include <utility>
#include <vector>

// TODO: Unverified
struct DirectedEulerianTour : public std::vector<int> {
  explicit DirectedEulerianTour(int n_, int s,
                                const std::vector<std::pair<int, int>> &edges_)
      : n{n_}, m(edges_.size()), head(n, -1), next(m), edges{edges_} {
    for (int i = 0; i < m; ++i) {
      auto [u, v] = edges[i];
      next[i] = head[v];
      head[u] = i;
    }
    dfs(s);
  }

private:
  void dfs(int v) {
    while (~head[v]) {
      int u = edges[head[v]].first;
      head[v] = next[head[v]];
      dfs(u);
    }
    push_back(v);
  }

  int n, m;
  std::vector<int> head, next;
  const std::vector<std::pair<int, int>> &edges;
};
