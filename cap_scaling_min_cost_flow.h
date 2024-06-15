#include "snippets/min_pow_of_two.h"
#include "snippets/min_pq.h"
#include "types/graph/adjacent_list_base.h"

#include <algorithm>
#include <cassert>
#include <concepts>
#include <functional>

template <std::integral CapT, typename CostT>
class CapScalingMinCostFlow
    : public AdjacentListBase<std::tuple<int, CapT, CostT>> {
  using Base = AdjacentListBase<std::tuple<int, CapT, CostT>>;
  using Base::edges;

  using OutFn = std::function<void(CapT, CostT)>;

  void update(int u, int v, int i, CapT delta) {
    excess[u] -= delta;
    excess[v] += delta;
    std::get<1>(edges[i]) -= delta;
    std::get<1>(edges[i ^ 1]) += delta;
  }

  bool augment(int s, int delta, const OutFn &output) {
    std::ranges::fill(visited, false);
    std::ranges::fill(dist, std::numeric_limits<CostT>::max());
    while (!pq.empty()) {
      pq.pop();
    }
    pq.emplace(dist[s] = 0, s);
    while (!pq.empty()) {
      auto [du, u] = pq.top();
      pq.pop();
      if (du == dist[u]) {
        visited[u] = true;
        if (excess[u] <= -delta) {
          // augment along the path s->t
          auto t = u;
          output(delta, dist[t] + pi[s] - pi[t]);
          for (int u = 0; u < n; u++) {
            if (visited[u]) {
              pi[u] = pi[u] - dist[u] + dist[t];
            }
          }
          for (int v = t; v != s;) {
            auto i = pre[v];
            assert(~i);
            auto u = std::get<0>(edges[i ^ 1]);
            update(u, v, i, delta);
            v = u;
          }
          return true;
        }
        for (int i = Base::head[u]; ~i; i = Base::next[i]) {
          auto [v, c, w] = edges[i];
          auto rw = w - pi[u] + pi[v];
          if (c >= delta && dist[v] > dist[u] + rw) {
            pre[v] = i;
            pq.emplace(dist[v] = dist[u] + rw, v);
          }
        }
      }
    }
    return false;
  }

  int n;
  CapT maxc = 1;
  std::vector<int> visited, pre;
  MinPQ<std::pair<CostT, int>> pq;
  std::vector<CapT> excess;
  std::vector<CostT> pi, dist;

public:
  explicit CapScalingMinCostFlow(int n_)
      : Base{n_}, n{n_}, visited(n), pre(n), excess(n), pi(n), dist(n) {}

  void add_edge(int u, int v, CapT c, CostT w) {
    Base::add(u, v, c, w);
    Base::add(v, u, 0, -w);
    maxc = std::max(maxc, c);
  }

  CapT operator()(int source, int sink, const OutFn &output) {
    auto delta = min_pow_of_two(maxc + 1) >> 1;
    CapT flow{0};
    while (delta) {
      for (int i = 0; i < static_cast<int>(edges.size()); i++) {
        auto &[v, c, w] = edges[i];
        auto u = std::get<0>(edges[i ^ 1]);
        if (c >= delta && w - pi[u] + pi[v] < 0) {
          update(u, v, i, delta);
          output(delta, w);
        }
      }
      for (int s = 0; s < n; s++) {
        while (excess[s] >= delta) {
          assert(augment(s, delta, output));
        }
      }
      do {
        flow += delta;
        excess[source] += delta;
        excess[sink] -= delta;
      } while (augment(source, delta, output));
      flow -= delta;
      excess[source] -= delta;
      excess[sink] += delta;
      delta >>= 1;
    }
    return flow;
  }
};
