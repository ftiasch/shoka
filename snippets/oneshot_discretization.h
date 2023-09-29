#include <algorithm>
#include <type_traits>
#include <vector>

template <typename T> int oneshot_discretization(std::vector<T> &a) {
  static_assert(std::is_integral_v<T>);
  int n = a.size();
  std::vector<std::pair<T, int>> ordered_a(n);
  for (int i = 0; i < n; i++) {
    ordered_a[i] = {a[i], i};
  }
  std::ranges::sort(ordered_a);
  int j = 0;
  if (n) {
    a[ordered_a[0].second] = 0;
    for (int i = 1; i < n; i++) {
      j += ordered_a[i - 1].first != ordered_a[i].first;
      a[ordered_a[i].second] = j;
    }
  }
  return j + 1;
}
