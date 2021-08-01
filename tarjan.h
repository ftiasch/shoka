#include <utility>
#include <vector>

struct Tarjan {
  Tarjan(int n, const std::vector<std::pair<int, int>> &edges)
      : dfn(n, -1), scc_id(n), low(n) {
    int dfs_count = 0;
    std::vector<int> stack;
    std::vector<std::vector<int>> graph(n);
    for (auto &&[u, v] : edges) {
      graph[u].push_back(v);
    }
    for (int r = 0; r < n; ++r) {
      dfs(graph, dfs_count, stack, r);
    }
  }

  int number_of_scc = 0;
  std::vector<int> dfn, scc_id;

private:
  void dfs(const std::vector<std::vector<int>> &graph, int &dfs_count,
           std::vector<int> &stack, int u) {
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
          scc_id[v = stack.back()] = number_of_scc;
          stack.pop_back();
        } while (u == v);
        number_of_scc++;
      }
    }
  }

  std::vector<int> low;
};
