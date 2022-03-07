#include <algorithm>
#include <limits>
#include <queue>
#include <utility>
#include <vector>

template <typename CostT> class SparsePrimalDual {
public:
  struct Result {
    int flow;
    CostT cost;
  };

  SparsePrimalDual(int n_, int m = 0)
      : n(n_), head(n, -1), pred(n), phi(n), dist(n) {
    edges.reserve(m << 1);
  }

  void add_edge(int u, int v, int capacity, CostT cost) {
    internal_add_edge(u, v, capacity, cost);
    internal_add_edge(v, u, 0, -cost);
  }

  Result augment(int source, int target,
                 int maxflow = std::numeric_limits<int>::max()) {
    const CostT MAX_COST = std::numeric_limits<CostT>::max();
    std::fill(dist.begin(), dist.end(), MAX_COST);
    PQ<std::pair<CostT, int>> pq;
    pq.emplace(dist[source] = 0, source);
    while (!pq.empty()) {
      auto [dist_u, u] = pq.top();
      pq.pop();
      if (dist_u >= dist[target]) {
        break;
      }
      if (dist_u == dist[u]) {
        for (int iterator = head[u]; ~iterator;
             iterator = edges[iterator].next) {
          auto &e = edges[iterator];
          int v = e.v;
          CostT cost = e.cost;
          if (e.rest > 0 && dist[v] > dist[u] + (phi[u] + cost - phi[v])) {
            dist[v] = dist[u] + (phi[u] + cost - phi[v]);
            pred[v] = iterator;
            pq.emplace(dist[v], v);
          }
        }
      }
    }
    if (dist[target] == MAX_COST) {
      return {0, 0};
    }
    int flow = maxflow;
    for (int v = target; v != source;) {
      flow = std::min(flow, edges[pred[v]].rest);
      v = edges[pred[v] ^ 1].v;
    }
    for (int v = target; v != source;) {
      edges[pred[v]].rest -= flow;
      edges[pred[v] ^ 1].rest += flow;
      v = edges[pred[v] ^ 1].v;
    }
    CostT cost = dist[target] + phi[target] - phi[source];
    for (int u = 0; u < n; ++u) {
      if (dist[u] < dist[target]) {
        phi[u] += dist[u];
      }
    }
    phi[target] += dist[target];
    return {flow, cost};
  }

private:
  template <typename T>
  using PQ = std::priority_queue<T, std::vector<T>, std::greater<T>>;

  struct Edge {
    int v, next, rest;
    CostT cost;
  };

  void internal_add_edge(int u, int v, int capacity, CostT cost) {
    edges.push_back(Edge{v, head[u], capacity, cost});
    head[u] = edges.size() - 1;
  }

  int n;
  std::vector<int> head, pred;
  std::vector<CostT> phi, dist;
  std::vector<Edge> edges;
};
