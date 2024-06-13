#include "types/graph.h"

#include <climits>
#include <vector>

template <IsGraph Graph> class DomTree {
  void prepare(int u) {
    visited[u] = true;
    dfn[u] = order.size();
    order.push_back(u);
    for (int v : graph[u]) {
      if (!visited[v]) {
        parent[v] = u;
        prepare(v);
      }
    }
  }

  int mplus(int u, int v) const { return sdom_dfn[u] < sdom_dfn[v] ? u : v; }

  int find(int u) {
    auto p = dsu[u].first;
    if (p != u) {
      find(p);
      dsu[u] = {dsu[p].first, mplus(dsu[p].second, dsu[u].second)};
    }
    return dsu[u].second;
  }

  const Graph &graph;
  int n;
  std::vector<bool> visited;
  std::vector<int> parent, dfn, sdom_dfn, sdom_head, sdom_next;
  std::vector<std::pair<int, int>> dsu;

public:
  explicit DomTree(const Graph &graph_, const Graph &rgraph, int source = 0)
      : graph{graph_}, n(graph.size()), visited(n), parent(n, -1), dfn(n),
        sdom_dfn(n, n), sdom_head(n, -1), sdom_next(n), dsu(n), idom(n, -1) {
    order.reserve(n);
    prepare(source);
    sdom_dfn[source] = n;
    for (int i = 0; i < n; i++) {
      dsu[i] = {i, source};
    }
    for (int v : order | std::views::drop(1) | std::views::reverse) {
      for (int u : rgraph[v]) {
        if (visited[u]) {
          sdom_dfn[v] = std::min(sdom_dfn[v],
                                 dfn[u] <= dfn[v] ? dfn[u] : sdom_dfn[find(u)]);
        }
      }
      dsu[v] = {parent[v], v};
      // add sdom[v] -> v
      auto sdom = order[sdom_dfn[v]];
      sdom_next[v] = sdom_head[sdom];
      sdom_head[sdom] = v;
      for (int u = sdom_head[parent[v]]; ~u; u = sdom_next[u]) {
        idom[u] = find(u);
      }
      sdom_head[parent[v]] = -1;
    }
    for (int v : order | std::views::drop(1)) {
      idom[v] =
          sdom_dfn[v] <= sdom_dfn[idom[v]] ? order[sdom_dfn[v]] : idom[idom[v]];
    }
  }

  std::vector<int> order, idom;
};
