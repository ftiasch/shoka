#include <type_traits>

template <typename T>
static constexpr std::enable_if_t<std::is_integral_v<T>, bool> is_prime(T n) {
  if (n < 2) {
    return false;
  }
  for (T d = 2; d * d <= n; ++d) {
    if (n % d == 0) {
      return false;
    }
  }
  return true;
}
