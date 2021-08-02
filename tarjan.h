#include <utility>
#include <vector>

class Tarjan {
  using Graph = std::vector<std::vector<int>>;

  static Graph to_graph(int n, const std::vector<std::pair<int, int>> &edges) {
    Graph graph(n);
    for (auto &&[u, v] : edges) {
      graph[u].push_back(v);
    }
    return graph;
  }

public:
  Tarjan(int n_, const Graph &graph) : n(n_), dfn(n_, -1), scc_id(n_), low(n_) {
    int dfs_count = 0;
    std::vector<int> stack;
    for (int r = 0; r < n_; ++r) {
      dfs(graph, dfs_count, stack, r);
    }
  }

  Tarjan(int n, const std::vector<std::pair<int, int>> &edges)
      : Tarjan(n, to_graph(n, edges)) {}

  int number_of_scc = 0;
  std::vector<int> dfn, scc_id;

private:
  void dfs(const Graph &graph, int &dfs_count, std::vector<int> &stack, int u) {
    if (dfn[u] == -1) {
      int tmp = dfn[u] = low[u] = dfs_count++;
      stack.push_back(u);
      for (int v : graph[u]) {
        dfs(graph, dfs_count, stack, v);
        tmp = std::min(tmp, low[v]);
      }
      low[u] = tmp;
      if (dfn[u] == low[u]) {
        int v;
        do {
          v = stack.back();
          stack.pop_back();
          scc_id[v] = number_of_scc;
          low[v] = n;
        } while (u != v);
        number_of_scc++;
      }
    }
  }

  int n;
  std::vector<int> low;
};
