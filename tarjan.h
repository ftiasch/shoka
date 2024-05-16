#include "types/graph.h"

#include <utility>
#include <vector>

template <IsGraph G>
  requires std::is_convertible_v<GraphEdge<G>, int>
class Tarjan {
public:
  explicit Tarjan(const G &graph_)
      : n(graph_.size()), graph(graph_), scc(n, -(n + 1)) {
    int clock = -n;
    std::vector<int> stack;
    for (int r = 0; r < n; ++r) {
      dfs(clock, stack, r);
    }
  }

  void dfs(int &clock, std::vector<int> &stack, int u) {
    if (scc[u] + n < 0) {
      int tmp, dfn;
      tmp = dfn = scc[u] = clock++;
      stack.push_back(u);
      for (int v : graph[u]) {
        dfs(clock, stack, v);
        tmp = std::min(tmp, scc[v]);
      }
      scc[u] = tmp;
      if (dfn == scc[u]) {
        int v;
        do {
          v = stack.back();
          stack.pop_back();
          scc[v] = num_scc;
        } while (u != v);
        num_scc++;
      }
    }
  }

  int n, num_scc = 0;
  const G &graph;
  std::vector<int> scc;
};
