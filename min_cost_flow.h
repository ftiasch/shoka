#include <algorithm>
#include <limits>
#include <queue>
#include <vector>

template <typename CostT> class MinCostFlow {
public:
  struct Result {
    int flow;
    CostT cost;
  };

  MinCostFlow(int n, int m = 0) : visited(n), head(n, -1), prev(n), dist(n) {
    edges.reserve(m << 1);
  }

  void add_edge(int u, int v, int capacity, CostT cost) {
    internal_add_edge(u, v, capacity, cost);
    internal_add_edge(v, u, 0, -cost);
  }

  Result augment(int src, int dst) {
    const CostT infdist = std::numeric_limits<CostT>::max();
    std::fill(dist.begin(), dist.end(), infdist);
    dist[src] = 0;
    std::queue<int> queue;
    queue.push(src);
    while (!queue.empty()) {
      int u = queue.front();
      queue.pop();
      visited[u] = false;
      for (int iter = head[u]; ~iter;) {
        auto &e = edges[iter];
        int v = e.v;
        if (e.rest && dist[u] + e.cost < dist[v]) {
          dist[v] = dist[u] + e.cost;
          prev[v] = iter;
          if (!visited[v]) {
            visited[v] = true;
            queue.push(v);
          }
        }
        iter = e.next;
      }
    }
    if (dist[dst] == infdist) {
      return Result{0, 0};
    }
    for (int v = dst; v != src; v = edges[prev[v] ^ 1].v) {
      edges[prev[v]].rest--;
      edges[prev[v] ^ 1].rest++;
    }
    return Result{1, dist[dst]};
  }

private:
  struct Edge {
    int v, next, rest;
    CostT cost;
  };

  void internal_add_edge(int u, int v, int capacity, CostT cost) {
    edges.push_back(Edge{v, head[u], capacity, cost});
    head[u] = edges.size() - 1;
  }

  std::vector<bool> visited;
  std::vector<int> head, prev;
  std::vector<CostT> dist;
  std::vector<Edge> edges;
};
