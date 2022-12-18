#include <array>

namespace nd_array {

template <typename T, int... NS> struct NDArrayFactory;

template <typename T, int N0, int... NS> struct NDArrayFactory<T, N0, NS...> {
  using Array = std::array<typename NDArrayFactory<T, NS...>::Array, N0>;
};

template <typename T, int N0> struct NDArrayFactory<T, N0> {
  using Array = std::array<T, N0>;
};

} // namespace nd_array

template <typename T, int... NS>
using NDArray = typename nd_array::NDArrayFactory<T, NS...>::Array;
