#pragma once

#include <experimental/type_traits>
#include <tuple>
#include <vector>

#ifndef YES
#define YES "Yes"
#endif
#ifndef NO
#define NO "No"
#endif

template <typename IO> struct IOBaseT {
  template <typename T = int> T read() {
    T v;
    static_cast<IO *>(this)->template read1(v);
    return v;
  }

  template <typename T = int> std::vector<T> read_v(size_t n) {
    std::vector<T> vec(n);
    for (auto &&v : vec) {
      v = read<T>();
    }
    return vec;
  }

  template <typename... Ts> std::tuple<Ts...> read_t() {
    std::tuple<Ts...> t;
    read_t_(t, std::make_index_sequence<sizeof...(Ts)>());
    return t;
  }

  IOBaseT &operator<<(bool o) { return *this << (o ? YES : NO); }

  template <std::ranges::forward_range RangeT>
  IOBaseT &operator<<(RangeT &&o)
    requires(!std::same_as<std::ranges::range_value_t<RangeT>, char>)
  {
    bool first = true;
    for (auto &&v : o) {
      if (first) {
        first = false;
      } else {
        *this << ' ';
      }
      *this << v;
    }
    return *this;
  }

  template <typename T> IOBaseT &operator<<(T &&o) {
    return static_cast<IO *>(this)->template write1(std::forward<T>(o)), *this;
  }

private:
  template <typename Tuple, std::size_t... Index>
  void read_t_(Tuple &t, std::index_sequence<Index...>) {
    (..., (std::get<Index>(t) = read<std::tuple_element_t<Index, Tuple>>()));
  }
};
