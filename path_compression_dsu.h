#include <vector>

class PathCompressionDsu : public std::vector<int> {
public:
  explicit PathCompressionDsu(int n) : std::vector<int>(n, -1) {}

  int find(int u) {
    auto &U = operator[](u);
    if (U == -1) {
      return u;
    }
    if (U != u) {
      U = find(U);
    }
    return U;
  }

  bool merge(int a, int b) {
    if (find(a) == find(b)) {
      return false;
    }
    operator[](find(a)) = find(b);
    return true;
  }
};
