#include <algorithm>
#include <cstdint>

namespace universal_euclidean {

using u64 = uint64_t;

template <typename MonoidT> MonoidT power(MonoidT a, u64 n) {
  MonoidT result = MonoidT::identity();
  while (n) {
    if (n & 1) {
      result = result * a;
    }
    a = a * a;
    n >>= 1;
  }
  return result;
}

// sum_{i = 0}^n (ai + b) / c
template <typename MonoidT> MonoidT sum(u64 n, u64 a, u64 b, u64 c) {
  MonoidT r = MonoidT::R();
  MonoidT u = MonoidT::U();
  MonoidT prefix = power(u, b / c) * r;
  MonoidT suffix = MonoidT::identity();
  b %= c;
  while (true) {
    if (a >= c) {
      r = power(u, a / c) * r;
      a %= c;
    } else {
      u64 m = (a * n + b) / c;
      if (m == 0) {
        return prefix * power(r, n) * suffix;
      }
      prefix = prefix * power(r, (c - b - 1) / a) * u;
      suffix = power(r, n - (c * m - b - 1) / a) * suffix;
      b = (c - b - 1) % a;
      std::swap(a, c);
      n = m - 1;
      std::swap(u, r);
    }
  }
}

} // namespace universal_euclidean
