#pragma once

#include <algorithm>
#include <tuple>
#include <utility>

namespace zip_with_details {

template <std::size_t Index, typename Func, typename... Ts>
constexpr decltype(auto) invoke_at(Func &&func, Ts &&...tuples) {
  return func(std::get<Index>(std::forward<Ts>(tuples))...);
}

template <std::size_t... Indices, typename Func, typename... Ts>
constexpr decltype(auto)
make_tuple(Func &&func, std::index_sequence<Indices...>, Ts &&...tuples) {
  return std::make_tuple(invoke_at<Indices>(std::forward<Func>(func),
                                            std::forward<Ts>(tuples)...)...);
}

} // namespace zip_with_details

template <typename Func, typename... Ts>
static constexpr decltype(auto) zip_with(Func &&func, Ts &&...tuples) {
  constexpr auto size =
      std::min({std::tuple_size_v<std::remove_reference_t<Ts>>...});
  return zip_with_details::make_tuple(std::forward<Func>(func),
                                      std::make_index_sequence<size>{},
                                      std::forward<Ts>(tuples)...);
}
