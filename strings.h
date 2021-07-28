#pragma once

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

template <int N> struct PrefixTable {
  void compute(int n, const char *s) {
    z[0] = 0;
    for (int i = 1, j = 0; i < n; ++i) {
      z[i] = i < j + z[j] ? std::min(z[i - j], j + z[j] - i) : 0;
      while (i + z[i] < n && s[z[i]] == s[i + z[i]]) {
        z[i]++;
      }
      if (i + z[i] > j + z[j]) {
        j = i;
      }
    }
    z[0] = n;
  }

  int z[N];
};

struct DefaultCharRank {
  int operator[](int i) const { return s[i] - 'a'; }
  const char *s;
};

template <int _N, int C, typename CharRank = DefaultCharRank> struct SA {
  static const int N = _N;

  void compute(int _n, const char *s) {
    n = _n;
    sort(C, IdOrder{}, CharRank{s}, sa);
    int range = rank(CharRank{s}, CharRank{s});
    for (int length = 1; length < n && range < n; length <<= 1) {
      memcpy(trk, rk, sizeof(*rk) * n);
      TRank r1{n, length, trk}, r0{n, 0, trk};
      sort(range + 1, IdOrder{}, r1, tsa);
      sort(range + 1, TOrder{tsa}, r0, sa);
      range = rank(r0, r1);
    }
  }

  int sa[N], rk[N];

private:
  struct IdOrder {
    int operator[](int i) const { return i; }
  };

  struct TOrder {
    int operator[](int i) const { return sa[i]; }
    const int *sa;
  };

  struct TRank {
    int operator[](int i) const {
      return i + shift < n ? rk[i + shift] + 1 : 0;
    }
    int n, shift;
    const int *rk;
  };

  template <typename Order, typename Rank>
  void sort(int range, const Order &o, const Rank &r, int *out) {
    memset(count, 0, sizeof(*count) * range);
    for (int i = 0; i < n; ++i) {
      count[r[i]]++;
    }
    for (int i = 1; i < range; ++i) {
      count[i] += count[i - 1];
    }
    for (int i = n; i--;) {
      out[--count[r[o[i]]]] = o[i];
    }
  }

  template <typename Rank0, typename Rank1>
  int rank(const Rank0 &r0, const Rank1 &r1) {
    rk[sa[0]] = 0;
    for (int _ = 1; _ < n; ++_) {
      int i = sa[_];
      int j = sa[_ - 1];
      rk[i] = rk[j] + (r0[i] != r0[j] || r1[i] != r1[j]);
    }
    return rk[sa[n - 1]] + 1;
  }

  int n;
  int count[std::max(N, C)], tsa[N], trk[N];
};

template <typename SA, template <typename, int> class RMQT>
struct LCPTable : SA {
  void compute(int _n, const char *s) {
    n = _n, SA::compute(n, s);
    int lcp = 0;
    for (int i = 0; i < n; ++i) {
      if (SA::rk[i]) {
        int j = SA::sa[SA::rk[i] - 1];
        while (i + lcp < n && j + lcp < n && s[i + lcp] == s[j + lcp]) {
          lcp++;
        }
        height[SA::rk[i] - 1] = lcp;
        lcp = std::max(lcp - 1, 0);
      }
    }
    rmq.compute(n - 1, height);
  }

  int lcp(int i, int j) const {
    if (i == j) {
      return n - i;
    }
    i = SA::rk[i], j = SA::rk[j];
    return i < j ? rmq.rmq(i, j) : rmq.rmq(j, i);
  }

private:
  static const int N = SA::N;

  int n, height[N - 1];
  RMQT<int, N - 1> rmq;
};

template <typename T, int N> struct SparseTable {
  SparseTable() {
    log[1] = 0;
    for (int i = 2; i <= N; ++i) {
      log[i] = 31 - __builtin_clz(i - 1);
    }
  }

  void compute(int n, const T *value) {
    l = log2n(n), memcpy(st[0], value, sizeof(T) * n);
    for (int i = 1; i < l; ++i) {
      for (int j = 0; j + (1 << i) <= n; ++j) {
        st[i][j] = std::min(st[i - 1][j], st[i - 1][j + (1 << i - 1)]);
      }
    }
  }

  T rmq(int a, int b) const {
    const int l = log[b - a];
    return std::min(st[l][a], st[l][b - (1 << l)]);
  }

private:
  static constexpr int log2n(int n) { return 32 - __builtin_clz(n - 1); }

  static const int L = log2n(N);

  int l, log[N + 1];
  T st[L][N];
};

template <int N, int C> struct SAM {
  struct Node {
    int length;
    Node *parent, *go[C];
  };

  SAM() { memset(nodes, 0, sizeof(nodes)); }

  Node *root() { return nodes; }

  void reset() {
    memset(nodes, 0, sizeof(*nodes) * (node_count));
    node_count = 1;
  }

  Node *extend(Node *p, int c) {
    Node *np = new_node(p->length + 1);
    while (p && !p->go[c]) {
      p->go[c] = np;
      p = p->parent;
    }
    if (!p) {
      np->parent = root();
    } else {
      Node *q = p->go[c];
      if (p->length + 1 == q->length) {
        np->parent = q;
      } else {
        Node *nq = new_node(p->length + 1);
        memcpy(nq->go, q->go, sizeof(q->go));
        nq->parent = q->parent;
        np->parent = q->parent = nq;
        while (p && p->go[c] == q) {
          p->go[c] = nq;
          p = p->parent;
        }
      }
    }
    return np;
  }

  int node_count = 1;
  Node nodes[N << 1];

private:
  Node *new_node(int length) { return nodes + (node_count++); }
};
