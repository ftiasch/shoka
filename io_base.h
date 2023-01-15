#pragma once

#include <tuple>
#include <type_traits>
#include <vector>

namespace io {

template <template <typename...> class Template, typename T>
struct is_specialization_of : std::false_type {};

template <template <typename...> class Template, typename... Args>
struct is_specialization_of<Template, Template<Args...>> : std::true_type {};

template <template <typename...> class Template, typename T>
inline constexpr bool is_specialization_of_v =
    is_specialization_of<Template, T>::value;

} // namespace io

template <typename IO> struct IOBaseT {
  template <typename T_ = int> std::decay_t<T_> read(T_ &&v = int{}) {
    using T = std::decay_t<T_>;
    if constexpr (io::is_specialization_of_v<std::tuple, T>) {
      readn_(v, std::make_index_sequence<std::tuple_size_v<T>>());

    } else if constexpr (io::is_specialization_of_v<std::vector, T>) {
      for (auto &e : v) {
        read(e);
      }
    } else {
      reinterpret_cast<IO *>(this)->template read1(std::forward<T>(v));
    }
    return v;
  }

  // helper

  template <typename T = int> std::vector<T> read_v(int n) {
    return read(std::vector<T>(n));
  }

  template <typename... Ts> std::tuple<Ts...> read_t() {
    return read(std::tuple<Ts...>{});
  }

private:
  template <typename Tuple, std::size_t... Index>
  void readn_(Tuple &t, std::index_sequence<Index...>) {
    (..., (std::get<Index>(t) = read(std::tuple_element_t<Index, Tuple>{})));
  }
};
