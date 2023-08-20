#include <cstdint>
#include <map>

template <typename BaseT = int, BaseT B_ = 0, typename IndexT = int>
struct TrygubNumber {
  explicit TrygubNumber(BaseT b_ = 0) : b{b_} {}

  constexpr BaseT base() const {
    if constexpr (B_) {
      return B_;
    } else {
      return b;
    }
  }

  // += c * B^i
  void add(int64_t c, IndexT i) {
    while (c != 0) {
      c += digits[i];
      auto t = c / base();
      c -= t * base();
      if ((digits[i] = c) == 0) {
        digits.erase(i);
      }
      c = t;
      i++;
    }
  }

  int signum() const {
    auto it = digits.rbegin();
    if (it == digits.rend()) {
      return 0;
    }
    return it->second > 0 ? 1 : -1;
  }

  BaseT operator[](IndexT i) const {
    auto it = digits.lower_bound(i);
    auto v = it == digits.end() || i != it->first ? 0 : it->second;
    if (it != digits.begin() && std::prev(it)->second < 0) {
      v--;
    }
    return v < 0 ? v + base() : v;
  }

  BaseT most_significant_digit() const {
    auto i = digits.rbegin()->first;
    while (!(*this)[i]) {
      i--;
    }
    return i;
  }

  BaseT b;
  std::map<IndexT, BaseT> digits;
};
