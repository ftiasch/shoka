#pragma once

#include "is_specialization_of.h"

#include <experimental/type_traits>
#include <string>
#include <tuple>
#include <type_traits>
#include <vector>

#ifndef YES
#define YES "Yes"
#endif
#ifndef NO
#define NO "No"
#endif

template <typename IO> struct IOBaseT {
  template <typename T = int> T read(T &&v = T{}) {
    using DecayedT = std::decay_t<T>;
    if constexpr (is_specialization_of_v<std::tuple, DecayedT>) {
      read_t_(v, std::make_index_sequence<std::tuple_size_v<DecayedT>>());
    } else if constexpr (is_vector_like<DecayedT>()) {
      for (auto it = v.begin(); it != v.end(); it++) {
        read(*it);
      }
    } else {
      static_cast<IO *>(this)->template read1(std::forward<T>(v));
    }
    return v;
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

  // helper

  template <typename T = int> std::vector<T> read_v(int n) {
    return read(std::vector<T>(n));
  }

  template <typename... Ts> std::tuple<Ts...> read_t() {
    return read(std::tuple<Ts...>{});
  }

private:
  template <typename T> using has_begin_t = decltype(std::declval<T>().begin());

  template <typename T> static constexpr bool is_vector_like() {
    return !std::is_same_v<T, std::string> &&
           std::experimental::is_detected_v<has_begin_t, T>;
  }

  template <typename Tuple, std::size_t... Index>
  void read_t_(Tuple &t, std::index_sequence<Index...>) {
    (..., (std::get<Index>(t) = read(std::tuple_element_t<Index, Tuple>{})));
  }
};
