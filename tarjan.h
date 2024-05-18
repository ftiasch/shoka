#include "types/graph.h"

#include <vector>

template <IsGraph G>
  requires std::is_convertible_v<GraphEdge<G>, int>
class Tarjan {
public:
  void dfs(int u) {
    if (scc[u] + n < 0) {
      int tmp, dfn;
      tmp = dfn = scc[u] = now++;
      stk[top++] = u;
      for (int v : g[u]) {
        dfs(v);
        tmp = std::min(tmp, scc[v]);
      }
      scc[u] = tmp;
      if (dfn == scc[u]) {
        do {
          scc[stk[--top]] = num_scc;
        } while (stk[top] != u);
        num_scc++;
      }
    }
  }

  const G &g;
  int n, now, top;
  std::vector<int> stk;

public:
  explicit Tarjan(const G &g_)
      : g{g_}, n(g.size()), now{-n}, top{0}, stk(n), num_scc{0},
        scc(n, -(n + 1)) {
    for (int r = 0; r < n; ++r) {
      dfs(r);
    }
  }

  int num_scc;
  std::vector<int> scc;
};
