#include <iostream>
#include <list>
#include <map>
#include <queue>
#include <set>
#include <type_traits>
#include <utility>
#include <vector>

namespace {

template <typename Tuple, size_t... Index>
std::ostream &serialize_tuple(std::ostream &out, const Tuple &t,
                              std::index_sequence<Index...>) {
  out << "(";
  (..., (out << (Index == 0 ? "" : ", ") << std::get<Index>(t)));
  return out << ")";
}

template <typename C> struct is_std_container : std::false_type {};
template <typename T, typename A>
struct is_std_container<std::vector<T, A>> : std::true_type {};
template <typename T, size_t N>
struct is_std_container<std::array<T, N>> : std::true_type {};
template <typename T, typename A>
struct is_std_container<std::list<T, A>> : std::true_type {};
template <typename T, typename A>
struct is_std_container<std::deque<T, A>> : std::true_type {};
template <typename K, typename C, typename A>
struct is_std_container<std::set<K, C, A>> : std::true_type {};
template <typename K, typename C, typename A>
struct is_std_container<std::multiset<K, C, A>> : std::true_type {};

} // namespace

template <typename A, typename B>
std::ostream &operator<<(std::ostream &out, const std::pair<A, B> &v) {
  return out << "(" << v.first << ", " << v.second << ")";
}

template <typename... T>
std::ostream &operator<<(std::ostream &out, const std::tuple<T...> &t) {
  return serialize_tuple(out, t, std::make_index_sequence<sizeof...(T)>());
}

template <typename Container>
typename std::enable_if<is_std_container<Container>::value,
                        std::ostream &>::type
operator<<(std::ostream &out, const Container &v) {
  out << "[";
  bool first = true;
  for (auto &&e : v) {
    if (first) {
      first = false;
    } else {
      out << ", ";
    }
    out << e;
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

template <typename K, typename V>
std::ostream &operator<<(std::ostream &out, const std::map<K, V> &m) {
  out << "{";
  bool first = true;
  for (auto &&[k, v] : m) {
    if (first) {
      first = false;
    } else {
      out << ", ";
    }
    out << k << ": " << v;
  }
  return out << "}";
}

#define KV(x) #x << "=" << x << ";"
