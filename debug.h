#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <ranges>
#include <set>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

template <typename Tuple, size_t... Index>
static inline std::ostream &serialize_tuple(std::ostream &out, const Tuple &t,
                                            std::index_sequence<Index...>) {
  out << "(";
  (..., (out << (Index == 0 ? "" : ", ") << std::get<Index>(t)));
  return out << ")";
}

} // namespace

template <typename... T>
std::ostream &operator<<(std::ostream &out, const std::tuple<T...> &t) {
  return serialize_tuple(out, t, std::make_index_sequence<sizeof...(T)>());
}

template <typename A, typename B>
std::ostream &operator<<(std::ostream &out, const std::pair<A, B> &v) {
  return out << std::tuple<A, B>(v.first, v.second);
}

template <std::ranges::forward_range RangeT>
std::ostream &operator<<(std::ostream &out, RangeT &&range) requires(
    !std::same_as<std::ranges::range_value_t<RangeT>, char>) {
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
std::ostream &operator<<(std::ostream &out, std::priority_queue<T, S, C> pq) {
  std::vector<T> v;
  while (!pq.empty()) {
    v.push_back(pq.top());
    pq.pop();
  }
  return out << v;
}

#define KV(x) #x << "=" << (x) << ";"
#define KV1(x) #x << "=" << (x) + 1 << ";"
