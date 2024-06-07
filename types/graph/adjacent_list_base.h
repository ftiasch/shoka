#pragma once

#include <vector>

template <typename Edge = int> class AdjacentListBase {
public:
  explicit AdjacentListBase(int n_, int m = 0) : n{n_}, head(n, -1) {
    next.reserve(m);
    edges.reserve(m);
  }

  template <typename... Args> void add(int u, Args &&...args) {
    int i = next.size();
    next.emplace_back(head[u]);
    head[u] = i;
    edges.emplace_back(std::forward<Args>(args)...);
  }

  int n;
  std::vector<int> head, next;
  std::vector<Edge> edges;
};
