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
    std::ranges::sort(*this, Compare{});
    this->erase(std::ranges::unique(*this).begin(), end());
  }

  int index(T x) const {
    return std::ranges::lower_bound(*this, x, Compare{}) - begin();
  }
};
