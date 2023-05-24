#pragma once

#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#if (__cplusplus >= 202002L)
#include <concepts>
#include <ranges>
#endif

template <typename T> struct Binary {
  static_assert(std::is_integral_v<T>);

  explicit Binary(T value_, int length_ = std::numeric_limits<T>::digits)
      : value{value_}, length{length_} {}

  T value;
  int length;
};

namespace std {

template <typename... T>
ostream &operator<<(ostream &out, const tuple<T...> &t) {
  out << '(';
  std::apply(
      [&out](const T &...args) {
        int index = 0;
        ((out << args << (++index != sizeof...(T) ? ", " : "")), ...);
      },
      t);
  return out << ')';
}

template <typename A, typename B>
ostream &operator<<(ostream &out, const pair<A, B> &v) {
  return out << tuple<A, B>(v.first, v.second);
}

template <typename T> ostream &operator<<(ostream &out, const Binary<T> &b) {
  out << "(";
#if (__cplusplus >= 202002L)
  for (auto i : std::ranges::iota_view(0, b.length)) {
#else
  for (int i = 0; i < b.length; ++i) {
#endif
    out << (b.value >> i & 1);
  }
  return out << ")_2";
}

#if (__cplusplus >= 202002L)
template <ranges::forward_range RangeT>
ostream &operator<<(ostream &out, RangeT &&range)
  requires(!same_as<ranges::range_value_t<RangeT>, char>)
{
  out << "[";
  bool first = true;
  for (auto &&elem : range) {
    if (first) {
      first = false;
    } else {
      out << ", ";
    }
    out << elem;
  }
  return out << "]";
}

template <typename T, typename S, typename C>
ostream &operator<<(ostream &out, priority_queue<T, S, C> pq) {
  vector<T> v;
  while (!pq.empty()) {
    v.push_back(pq.top());
    pq.pop();
  }
  return out << v;
}
#endif

} // namespace std

struct DebugLine {
  explicit DebugLine(int lineno) { std::cerr << lineno << "L "; }

  ~DebugLine() { std::cerr << std::endl; }

  template <typename T> DebugLine &operator<<(T &&v) {
    std::cerr << std::forward<T>(v);
    return *this;
  }
};

#define KV(x) #x "=" << (x) << ";"
#define DEBUG DebugLine(__LINE__)
