#include <utility>
#include <vector>

class Tarjan {
public:
  using Graph = std::vector<std::vector<int>>;

  static Graph to_graph(int n, const std::vector<std::pair<int, int>> &edges) {
    Graph graph(n);
    for (auto &&[u, v] : edges) {
      graph[u].push_back(v);
    }
    return graph;
  }

  Tarjan(int n_, const Graph &graph_)
      : n(n_), dfn(n, -1), scc_id(n), low(n), graph(graph_) {
    int dfs_count = 0;
    std::vector<int> stack;
    for (int r = 0; r < n_; ++r) {
      dfs(dfs_count, stack, r);
    }
  }

  Graph scc_graph() const {
    Graph result(number_of_scc);
    for (int u = 0; u < n; ++u) {
      for (int v : graph[u]) {
        if (scc_id[u] != scc_id[v]) {
          result[scc_id[u]].push_back(scc_id[v]);
        }
      }
    }
    return result;
  }

  int n, number_of_scc = 0;
  std::vector<int> dfn, scc_id;

private:
  void dfs(int &dfs_count, std::vector<int> &stack, int u) {
    if (dfn[u] == -1) {
      int tmp = dfn[u] = low[u] = dfs_count++;
      stack.push_back(u);
      for (int v : graph[u]) {
        dfs(dfs_count, stack, v);
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

  std::vector<int> low;
  const Graph &graph;
};
