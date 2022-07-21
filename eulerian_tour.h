#include <utility>
#include <vector>

struct EulerianTour : public std::vector<int> {
  explicit EulerianTour(int n_, int s,
                        const std::vector<std::pair<int, int>> &edges_)
      : n{n_}, m(edges_.size()), head(n, -1), next(m << 1),
        used(m), edges{edges_} {
    for (int i = 0; i < m << 1; ++i) {
      auto [u, v] = edges[i >> 1];
      if (i & 1) {
        std::swap(u, v);
      }
      next[i] = head[u];
      head[u] = i;
    }
    dfs(s);
  }

private:
  void dfs(int u) {
    while (~head[u]) {
      int i = head[u];
      head[u] = next[head[u]];
      if (!used[i >> 1]) {
        used[i >> 1] = true;
        int v = (i & 1) ? edges[i >> 1].first : edges[i >> 1].second;
        dfs(v);
      }
    }
    push_back(u);
  }

  int n, m;
  std::vector<int> head, next;
  std::vector<bool> used;
  const std::vector<std::pair<int, int>> &edges;
};
