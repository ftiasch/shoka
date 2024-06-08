#include "doubling_lca_base.h"
#include "types/topo_graph.h"

template <IsGraph Graph> class DagDomTree : public DoublingLcaBase {
public:
  explicit DagDomTree(const TopoGraph<Graph> &dag)
      : DoublingLcaBase(dag.size()), idom(dag.size(), -1) {
    for (int u : dag.rorder | std::views::reverse) {
      if (~idom[u]) {
        add_leaf(u, idom[u]);
      }
      for (int v : dag[u]) {
        idom[v] = ~idom[v] ? lca(idom[v], u) : u;
      }
    }
  }

  std::vector<int> idom;
};
