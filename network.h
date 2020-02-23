namespace network {

template <typename IntType> class IntValue {
public:
  using ValueType = IntType;

  static bool greater_than_zero(ValueType x) { return x > 0; }
};

template <int PRECISION> class DoubleValue {
public:
  using ValueType = double;
  constexpr static double EPSILON = pow(10, -PRECISION);

  static bool greater_than_zero(ValueType x) { return x > EPSILON; }
};

} // namespace network

template <typename Derived = network::IntValue<int>> class NetworkDinic {
public:
  using ValueType = typename Derived::ValueType;

  NetworkDinic(int n, int m = 0) : n(n), head(n, -1), level(n), current(n) {
    edges.reserve(m);
  }

  void add(int f, int t, ValueType c, bool back = true) {
    add_edge(f, t, c);
    if (back) {
      add_edge(t, f, static_cast<ValueType>(0));
    }
  }

  ValueType max_flow(int source, int target) {
    ValueType result = 0;
    while (bfs(source, target)) {
      std::copy(head.begin(), head.end(), current.begin());
      result += dfs(source, target, std::numeric_limits<ValueType>::max());
    }
    return result;
  }

private:
  struct Edge {
    int t;
    int next;
    ValueType c;
  };

  void add_edge(int f, int t, ValueType c) {
    edges.push_back(Edge{t, head[f], c});
    head[f] = static_cast<int>(edges.size()) - 1;
  }

  bool bfs(int source, int target) {
    std::fill(level.begin(), level.end(), -1);
    level[target] = 0;
    std::vector<int> queue{target};
    for (int h = 0; h < static_cast<int>(queue.size()) && !~level[source];
         ++h) {
      int u = queue[h];
      for (int iterator = head[u]; ~iterator; iterator = edges[iterator].next) {
        int v = edges[iterator].t;
        if (greater_than_zero(edges[iterator ^ 1].c) && !~level[v]) {
          level[v] = level[u] + 1;
          queue.push_back(v);
        }
      }
    }
    return ~level[source];
  }

  ValueType dfs(int u, int target, ValueType remain) {
    if (u == target) {
      return remain;
    }
    auto argumented = static_cast<ValueType>(0);
    for (auto &iterator = current[u]; ~iterator && greater_than_zero(remain);) {
      auto &e = edges[iterator];
      if (greater_than_zero(e.c) && level[u] == level[e.t] + 1) {
        auto tmp = dfs(e.t, target, std::min(e.c, remain));
        e.c -= tmp;
        edges[iterator ^ 1].c += tmp;
        remain -= tmp;
        argumented += tmp;
        if (!greater_than_zero(remain)) {
          return argumented;
        }
      }
      iterator = e.next;
    }
    level[u] = -1;
    return argumented;
  }

  static bool greater_than_zero(ValueType x) {
    return Derived::greater_than_zero(x);
  }

  int n;
  std::vector<int> head, level, current;
  std::vector<Edge> edges;
};

class MinCostFlow {
public:
  struct Result {
    int flow, cost;
  };

  MinCostFlow(int n, int m = 0) : visited(n), head(n, -1), dist(n), prev(n) {
    edges.reserve(m << 1);
  }

  void add_edge(int u, int v, int capacity, int cost) {
    internal_add_edge(u, v, capacity, cost);
    internal_add_edge(v, u, 0, -cost);
  }

  Result augment(int src, int dst) {
    const int infdist = std::numeric_limits<int>::max();
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
    int v, next, rest, cost;
  };

  void internal_add_edge(int u, int v, int capacity, int cost) {
    edges.push_back(Edge{v, head[u], capacity, cost});
    head[u] = edges.size() - 1;
  }

  std::vector<bool> visited;
  std::vector<int> head, dist, prev;
  std::vector<Edge> edges;
};
