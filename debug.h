#pragma once

#include <concepts>
#include <iostream>
#include <limits>
#include <list>
#include <map>
#include <queue>
#include <ranges>
#include <set>
#include <type_traits>
#include <utility>
#include <vector>

namespace debug {

template <typename Tuple, size_t... Index>
static inline std::ostream &serialize_tuple(std::ostream &out, const Tuple &t,
                                            std::index_sequence<Index...>) {
  out << "(";
  (..., (out << (Index == 0 ? "" : ", ") << std::get<Index>(t)));
  return out << ")";
}

} // namespace debug

template <typename T>
requires std::integral<T>
struct Binary {
  explicit Binary(T value_, int length_ = std::numeric_limits<T>::digits)
      : value{value_}, length{length_} {}

  T value;
  int length;
};

namespace std {

template <typename... T>
ostream &operator<<(ostream &out, const tuple<T...> &t) {
  return debug::serialize_tuple(out, t, make_index_sequence<sizeof...(T)>());
}

template <typename A, typename B>
ostream &operator<<(ostream &out, const pair<A, B> &v) {
  return out << tuple<A, B>(v.first, v.second);
}

template <ranges::forward_range RangeT>
ostream &operator<<(ostream &out, RangeT &&range) requires(
    !same_as<ranges::range_value_t<RangeT>, char>) {
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

template <typename T> ostream &operator<<(ostream &out, const Binary<T> &b) {
  out << "(";
  for (auto i : std::ranges::iota_view(0, b.length)) {
    out << (b.value >> i & 1);
  }
  return out << ")_2";
}

} // namespace std

#define KV(x) #x << "=" << (x) << ";"
#define KV1(x) #x << "=" << (x) + 1 << ";"
