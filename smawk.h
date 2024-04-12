#include <concepts>
#include <numeric>
#include <ranges>
#include <span>
#include <type_traits>
#include <vector>

template <typename A>
concept IsTM = requires(A m) {
  {
    m(std::declval<int>(), std::declval<int>())
  } -> std::convertible_to<typename A::E>;
  {
    std::declval<typename A::E>() > std::declval<typename A::E>()
  } -> std::convertible_to<bool>;
};

template <IsTM A> struct SMAWK {
  explicit SMAWK(int n_, int m_, const A &a_)
      : n{n_}, m{m_}, a{a_}, row_min(n), cols(m + 2 * n) {
    stack.reserve(n);
    std::iota(cols.begin(), cols.begin() + m, 0);
    recur(0, 0, m);
  }

  void recur(int k, int begin, int end) {
    if (n < (2 << k)) {
      auto r = (1 << k) - 1;
      row_min[r] = get(r + 1, cols[begin]);
      for (int i = begin + 1; i < end; i++) {
        check_min(row_min[r], get(r + 1, cols[i]));
      }
    } else {
      stack.clear();
      for (int i = begin; i < end; i++) {
        auto c = cols[i];
        while (!stack.empty() && stack.back() > get((stack.size() << k), c)) {
          stack.pop_back();
        }
        auto r1 = (stack.size() + 1) << k;
        if (r1 <= n) {
          stack.push_back(get(r1, c));
        }
      }
      for (int i = 0; i < stack.size(); i++) {
        cols[end + i] = stack[i].second;
      }
      begin = end, end += stack.size();
      recur(k + 1, begin, end);
      auto offset = 1 << k;
      for (int r = offset - 1, p = begin; r < n; r += offset << 1) {
        auto high = r + offset < n ? row_min[r + offset].second + 1 : m;
        row_min[r] = get(r + 1, cols[p]);
        while (p + 1 < end && cols[p + 1] < high) {
          check_min(row_min[r], get(r + 1, cols[++p]));
        }
      }
    }
  }

  using TP = std::pair<typename A::E, int>;

  TP get(int x1, int y) { return {a(x1 - 1, y), y}; }

  static void check_min(TP &x, TP a) {
    if (x > a) {
      x = a;
    }
  }

  int n, m;
  const A &a;
  std::vector<TP> row_min, stack;
  std::vector<int> cols;
};
