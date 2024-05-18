#pragma once

#include <concepts>
#include <ranges>

template <typename G>
concept IsGraph = requires(G g) {
  { g.size() } -> std::convertible_to<int>;
} && requires(G g, int u) {
  requires std::ranges::forward_range<decltype(g[u])>;
};

template <IsGraph G>
using GraphEdge = std::ranges::range_value_t<
    decltype(std::declval<G>()[std::declval<int>()])>;
