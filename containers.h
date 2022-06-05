#include <queue>
#include <vector>

template <typename T, int N> struct VectorND {
  using Nested = VectorND<T, N - 1>;
  using Vector = std::vector<typename Nested::Vector>;

  template <typename... Args> static Vector create(int n, Args &&...args) {
    return Vector(n, Nested::create(std::forward<Args>(args)...));
  }
};

template <typename T> struct VectorND<T, 1> {
  using Vector = std::vector<T>;

  template <typename... Args> static Vector create(Args &&...args) {
    return Vector(std::forward<Args>(args)...);
  }
};

template <typename T>
using MinPQ = std::priority_queue<T, std::vector<T>, std::greater<T>>;
