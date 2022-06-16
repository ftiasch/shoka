#include <array>

namespace details {

template <typename T, int... NS> struct NDArrayFactory;

template <typename T, int N0, int... NS> struct NDArrayFactory<T, N0, NS...> {
  using Array = std::array<typename NDArrayFactory<T, NS...>::Array, N0>;
};

template <typename T, int N0> struct NDArrayFactory<T, N0> {
  using Array = std::array<T, N0>;
};

} // namespace details

template <typename T, int... NS>
using NDArray = typename details::NDArrayFactory<T, NS...>::Array;
