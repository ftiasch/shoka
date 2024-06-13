#include "monoid_dsu.h"
#include "types/graph.h"

#include <climits>

template <IsGraph Graph> class DomTree {
  using Pair = std::pair<int, int>;

  struct Min {
    static Min plus(Min a, Min b) { return {std::min<Pair>(a, b)}; }

    operator Pair() const { return value; }

    Pair value{INT_MAX, INT_MAX};
  };

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

  const Graph &graph;
  int n;
  std::vector<bool> visited;
  std::vector<int> parent, dfn, sdom_dfn, sdom_head, sdom_next;

public:
  explicit DomTree(const Graph &graph_, const Graph &rgraph, int source = 0)
      : graph{graph_}, n(graph.size()), visited(n), parent(n, -1), dfn(n),
        sdom_dfn(n, n), sdom_head(n, -1), sdom_next(n), idom(n, -1) {
    order.reserve(n);
    prepare(source);
    MonoidDsu<Min> dsu(n);
    for (int v : order | std::views::drop(1) | std::views::reverse) {
      for (int u : rgraph[v]) {
        if (visited[u]) {
          sdom_dfn[v] = std::min(
              sdom_dfn[v], dfn[u] <= dfn[v]
                               ? dfn[u]
                               : static_cast<Pair>(dsu.find(u).second).first);
        }
      }
      dsu.link(v, parent[v], Min{Pair{sdom_dfn[v], v}});
      // add sdom[v] -> v
      auto sdom = order[sdom_dfn[v]];
      sdom_next[v] = sdom_head[sdom];
      sdom_head[sdom] = v;
      for (int u = sdom_head[parent[v]]; ~u; u = sdom_next[u]) {
        idom[u] = static_cast<Pair>(dsu.find(u).second).second;
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
