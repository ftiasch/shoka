#pragma once

#include <tuple>
#include <type_traits>
#include <vector>

template <typename IO> struct IOBaseT {
  template <typename... Ts> std::tuple<Ts...> read() {
    std::tuple<Ts...> t;
    read_tuple(t, std::make_index_sequence<sizeof...(Ts)>());
    return t;
  }

  template <typename T = int> std::vector<T> read_vector(int n) {
    std::vector<T> v(n);
    for (int i = 0; i < n; ++i) {
      v[i] = reinterpret_cast<IO *>(this)->template read1<T>();
    }
    return v;
  }

private:
  template <typename Tuple, std::size_t... Index>
  void read_tuple(Tuple &t, std::index_sequence<Index...>) {
    (..., (std::get<Index>(t) =
               reinterpret_cast<IO *>(this)
                   ->template read1<std::tuple_element_t<Index, Tuple>>()));
  }
};
