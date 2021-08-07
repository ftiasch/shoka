#include <queue>

template <int N, int C> struct AhoCorasick {
  AhoCorasick() : n(1) {
    memset(weight, 0, sizeof(weight));
    memset(go, -1, sizeof(go));
  }

  void insert(const char *s, int w = 1) {
    int p = 0;
    for (int i = 0; s[i]; ++i) {
      int c = s[i] - 'a';
      if (go[p][c] == -1) {
        go[p][c] = n++;
      }
      p = go[p][c];
    }
    weight[p] += w;
  }

  void initialize() {
    std::queue<int> queue;
    for (int c = 0; c < C; ++c) {
      int &v = go[0][c];
      if (~v) {
        queue.push(v);
      }
      (~v ? fail[v] : v) = 0;
    }
    while (!queue.empty()) {
      int u = queue.front();
      queue.pop();
      weight[u] += weight[fail[u]];
      for (int c = 0; c < C; ++c) {
        int &v = go[u][c];
        if (~v) {
          queue.push(v);
        }
        (~v ? fail[v] : v) = go[fail[u]][c];
      }
    }
  }

  int n, weight[N], go[N][C], fail[N];
};
