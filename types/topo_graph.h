#pragma once

#include "graph.h"

#include <vector>

template <IsGraph Dag> class TopoGraph : public Dag {
public:
  explicit TopoGraph(int n) : Dag(n), visited(n) { rorder.reserve(n); }

  void dfs(int u) {
    if (!visited[u]) {
      visited[u] = true;
      for (int v : (*this)[u]) {
        dfs(v);
      }
      rorder.push_back(u);
    }
  }

  std::vector<bool> visited;
  std::vector<int> rorder;
};
