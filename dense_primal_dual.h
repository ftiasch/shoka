#include <algorithm>
#include <exception>
#include <limits>
#include <utility>
#include <vector>

template <typename CostT> struct DensePrimalDual {
  using Result = std::pair<int, CostT>;

  DensePrimalDual(int n_)
      : n(n_), phi(n), net(n, std::vector<Edge>(n)), visited(n), dist(n),
        pred(n) {}

  // NOTE: at most one of the (u->v) and (v->u) edges exists
  void add_edge(int u, int v, int cap, CostT cost) {
    if (cost < 0) {
      throw std::runtime_error("cost must > 0");
    }
    // std::cerr << u << "->" << v << " [label=\"" << cost << "\"]" <<
    // std::endl;
    net[u][v] = {cap, cost};
    net[v][u] = {0, -cost};
  }

  Result augment(int source, int target,
                 int maxflow = std::numeric_limits<int>::max()) {
    std::fill(visited.begin(), visited.end(), false);
    std::fill(dist.begin(), dist.end(), MAX_COST);
    dist[source] = 0;
    while (true) {
      std::pair<CostT, int> candidate{MAX_COST, -1};
      for (int u = 0; u < n; ++u) {
        if (!visited[u]) {
          candidate = std::min(candidate, {dist[u], u});
        }
      }
      int u = candidate.second;
      if (u == -1 || u == target) {
        break;
      }
      visited[u] = true;
      for (int v = 0; v < n; ++v) {
        auto [cap, cost] = net[u][v];
        if (cap > 0 && dist[v] > dist[u] + (phi[u] + cost - phi[v])) {
          dist[v] = dist[u] + (phi[u] + cost - phi[v]);
          pred[v] = u;
        }
      }
    }
    if (dist[target] == MAX_COST) {
      return {0, 0};
    }
    int flow = maxflow;
    for (int v = target; v != source;) {
      int u = pred[v];
      flow = std::min(flow, net[u][v].first);
      v = u;
    }
    for (int v = target; v != source;) {
      int u = pred[v];
      net[u][v].first -= flow;
      net[v][u].first += flow;
      v = u;
    }
    CostT cost = dist[target] + phi[target];
    for (int u = 0; u < n; ++u) {
      if (dist[u] < dist[target]) {
        phi[u] += dist[u];
      }
    }
    phi[target] += dist[target];
    return {flow, cost};
  }

private:
  static const CostT MAX_COST = std::numeric_limits<CostT>::max();

  using Edge = std::pair<int, CostT>;

  int n;
  std::vector<CostT> phi;
  std::vector<std::vector<Edge>> net;
  std::vector<bool> visited;
  std::vector<CostT> dist;
  std::vector<int> pred;
};
