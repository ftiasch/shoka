#include <algorithm>
#include <vector>

template <typename T, typename Compare = std::less<T>>
struct Discretization : public std::vector<T> {
  using std::vector<T>::begin;
  using std::vector<T>::end;

  explicit Discretization() = default;

  explicit Discretization(const std::vector<T> &a_) : std::vector<T>(a_) {
    normalize();
  }

  void normalize() {
    std::sort(begin(), end(), Compare{});
    std::vector<T>::erase(std::unique(begin(), end()), end());
  }

  int index(T x) const {
    return std::lower_bound(begin(), end(), x, Compare{}) - begin();
  }
};
