#pragma

#include <queue>

template <typename T>
using MinPQ = std::priority_queue<T, std::vector<T>, std::greater<>>;
