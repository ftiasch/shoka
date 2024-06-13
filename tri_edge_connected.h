#include "types/graph/adjacent_list_base.h"

#include <algorithm>
#include <random>
#include <vector>

class TriEdgeConnected : public AdjacentListBase<int> {
  void dfs(int u) {
    circ[u] = 0;
    for (int iterator = head[u]; ~iterator; iterator = next[iterator]) {
      auto v = edges[iterator];
      if (!used[iterator >> 1]) {
        used[iterator >> 1] = true;
        if (~circ[v]) {
          auto h = gen();
          cov[u]++;
          cov[v]--;
          circ[u] ^= h;
          circ[v] ^= h;
        } else {
          int pos = dfs_order.size();
          dfs_order.emplace_back(u, v);
          dfs(v);
          if (circ[v] == 0) {
            // (u, v) is a cut
            label[v] ^= gen();
          } else {
            if (cov[v] == 1) {
              // there's a back edge f where label[f] == label[(u, v)]
              label[v] ^= gen();
            }
            order.emplace_back(circ[v], pos, v);
          }
          cov[u] += cov[v];
          circ[u] ^= circ[v];
        }
      }
    }
  }

  int m;
  std::mt19937_64 &gen;
  std::vector<bool> used;
  std::vector<uint64_t> cov, circ;
  std::vector<std::pair<int, int>> dfs_order;
  std::vector<std::tuple<uint64_t, int, int>> order;

public:
  explicit TriEdgeConnected(int n,
                            const std::vector<std::pair<int, int>> &edges,
                            std::mt19937_64 &gen_)
      : AdjacentListBase{n, static_cast<int>(edges.size()) << 1},
        m(edges.size()), gen{gen_}, used(m), cov(n), circ(n, -1), label(n, 0) {
    for (auto [a, b] : edges) {
      add(a, b);
      add(b, a);
    }
    dfs_order.reserve(n);
    order.reserve(n);
    for (int u = 0; u < n; u++) {
      if (!~circ[u]) {
        dfs(u);
        label[u] ^= gen();
      }
    }
    std::ranges::sort(order);
    for (int i = 1; i < order.size(); i++) {
      if (std::get<0>(order[i - 1]) == std::get<0>(order[i])) {
        auto h = gen();
        label[std::get<2>(order[i - 1])] ^= h;
        label[std::get<2>(order[i])] ^= h;
      }
    }
    for (auto [u, v] : dfs_order) {
      label[v] ^= label[u];
    }
  }

  std::vector<uint64_t> label;
};
