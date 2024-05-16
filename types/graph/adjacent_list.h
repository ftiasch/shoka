#pragma once

#include <vector>

template <typename E = int> struct AdjacentListT {
  using Edge = std::pair<int, E>;
  using Edges = std::vector<Edge>;

  struct Iterator {
    using difference_type = int; // needs in std::input_iterator
    using value_type = E;

    E operator*() const { return (*edges)[it].second; }

    Iterator &operator++() {
      it = (*edges)[it].first;
      return *this;
    }

    Iterator operator++(int) {
      auto tmp = *this;
      it = (*edges)[it].first;
      return tmp;
    }

    bool operator==(const Iterator &o) const { return it == o.it; }

    int it;
    const Edges *edges;
  };

  struct ConstList {
    Iterator begin() const { return {head, &edges}; }
    Iterator end() const { return {-1, &edges}; }

    int head;
    const Edges &edges;
  };

  explicit AdjacentListT(int n_) : n{n_}, head(n, -1) {}
  int size() const { return n; }
  ConstList operator[](int u) const { return {head[u], edges}; }

  template <typename... Args> void add(int u, Args... args) {
    int i = edges.size();
    edges.emplace_back(head[u], E{std::forward<Args>(args)...});
    head[u] = i;
  }

  int n;
  std::vector<int> head;
  Edges edges;
};
