#include <vector>

class Dsu : public std::vector<int> {
public:
  explicit Dsu(int n) : std::vector<int>(n, -1) {}

  int find(int u) {
    if ((*this)[u] == -1) {
      return u;
    }
    if ((*this)[u] != u) {
      (*this)[u] = find((*this)[u]);
    }
    return (*this)[u];
  }

  bool merge(int a, int b) {
    if (find(a) == find(b)) {
      return false;
    }
    (*this)[find(a)] = find(b);
    return true;
  }
};
