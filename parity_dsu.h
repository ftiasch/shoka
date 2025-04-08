#pragma once

#include <utility>
#include <vector>

class ParityDsu {
  using Entry = std::pair<int, char>;

  std::vector<Entry> parent;

public:
  explicit ParityDsu(int n) {
    parent.reserve(n);
    for (int i = 0; i < n; i++) {
      parent.emplace_back(i, 0);
    }
  }

  Entry find(int u) {
    if (parent[u].first != u) {
      auto [r, c] = find(parent[u].first);
      parent[u] = {r, parent[u].second ^ c};
    }
    return parent[u];
  }

  int link(int u, int v, char c) {
    // 0 : Connected, OK
    // 1 : Disconnected, but OK
    // -1: Conflict
    auto [ru, cu] = find(u);
    auto [rv, cv] = find(v);
    auto cc = c ^ cu ^ cv;
    if (ru == rv) {
      return cc ? -1 : 0;
    }
    parent[ru] = {rv, cc};
    return 1;
  }
};
