#include <queue>
#include <vector>

template <typename T, int N>
struct VectorND : public std::vector<VectorND<T, N - 1>> {
  VectorND() {}

  template <typename... Args>
  VectorND(int dim0, Args &&...args)
      : std::vector<Nested>(dim0, Nested(std::forward<Args>(args)...)) {}

private:
  using Nested = VectorND<T, N - 1>;
};

template <typename T> struct VectorND<T, 1> : public std::vector<T> {
  template <typename... Args>
  VectorND(Args &&...args) : std::vector<T>(std::forward<Args>(args)...) {}
};

template <typename T> using Vector2D = VectorND<T, 2>;

template <typename T>
using MinPQ = std::priority_queue<T, std::vector<T>, std::greater<T>>;
