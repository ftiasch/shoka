#include "monoid_dsu.h"
#include "snippets/update_min.h"
#include "types/graph.h"

#include <climits>

template <IsGraph Graph> class DomTree {
  struct MinMonoid {
    static MinMonoid plus(MinMonoid a, MinMonoid b) {
      return {std::min<int>(a, b)};
    }

    operator int() const { return value; }

    int value = INT_MAX;
  };

  void prepare(int u) {
    visit[u] = 1;
    dfn[u] = order.size();
    order.push_back(u);
    for (int v : graph[u]) {
      if (visit[v] == 0) {
        parent[v] = u;
        prepare(v);
      }
    }
    visit[u] = 2;
  }

  void dfs(int u, int top) {
    visit[u] = 3;
    top = std::upper_bound(stack.begin(), stack.begin() + top, sdom[u]) -
          stack.begin();
    // cover (sdom[u], u)
    idom[u] = order[stack[top - 1]];
    auto backup = stack[top];
    stack[top] = dfn[u];
    for (int v : graph[u]) {
      if (visit[v] != 3) {
        dfs(v, top + 1);
      }
    }
    stack[top] = backup;
  }

  const Graph &graph;

  int n;
  std::vector<int> parent, visit, dfn, sdom, stack;
  // 0 - not visited
  // 1 - visiting
  // 2 - visited

public:
  explicit DomTree(const Graph &graph_, const Graph &rgraph, int source = 0)
      : graph{graph_}, n(graph.size()), parent(n, -1), visit(n), dfn(n),
        sdom(n, n), stack(n), idom(n, -1) {
    order.reserve(n);
    prepare(source);
    {
      MonoidDsu<MinMonoid> dsu(n);
      for (int v : order | std::views::reverse) {
        for (int u : rgraph[v]) {
          update_min(sdom[v],
                     dfn[u] <= dfn[v] ? dfn[u] : (int)dsu.find(u).second);
        }
        if (~parent[v]) {
          dsu.link(v, parent[v], MinMonoid{sdom[v]});
        }
      }
    }
    stack[0] = 0;
    visit[source] = 3;
    for (int v : graph[source]) {
      if (visit[v] != 3) {
        dfs(v, 1);
      }
    }
  }

  std::vector<int> order, idom;
};
