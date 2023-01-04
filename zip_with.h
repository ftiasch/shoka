#pragma once

#include <algorithm>
#include <tuple>
#include <type_traits>
#include <utility>

namespace zip_with_details {

template <std::size_t Index, typename Func, typename... Ts>
static inline constexpr decltype(auto) invoke_at(Func &&func, Ts &&...tuples) {
  return func(std::get<Index>(std::forward<Ts>(tuples))...);
}

template <std::size_t... Indices, typename Func, typename... Ts>
static inline constexpr decltype(auto)
make_tuple(Func &&func, std::index_sequence<Indices...>, Ts &&...tuples) {
  return std::make_tuple(invoke_at<Indices>(std::forward<Func>(func),
                                            std::forward<Ts>(tuples)...)...);
}

template <class T, class... Ts>
static inline constexpr bool are_same_v = (std::is_same_v<T, Ts> && ...);

} // namespace zip_with_details

template <typename Func, typename... Ts>
static constexpr decltype(auto) zip_with(Func &&func, Ts &&...tuples) {
  using namespace zip_with_details;
  static_assert(are_same_v<std::decay_t<Ts>...>);
  constexpr auto size = std::tuple_size_v<
      std::decay_t<std::tuple_element_t<0, std::tuple<Ts...>>>>;
  return make_tuple(std::forward<Func>(func), std::make_index_sequence<size>{},
                    std::forward<Ts>(tuples)...);
}
