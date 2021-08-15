#include <numeric>

template <typename T, unsigned N> struct MemPool {
  MemPool() : head(0), rear(0) { std::iota(free_list, free_list + N, 0); }

  T *allocate() { return memory + free_list[fetch_and_add(head)]; }

  void free(const T *object) {
    free_list[fetch_and_add(rear)] = object - memory;
  }

private:
  static unsigned fetch_and_add(unsigned &u) {
    unsigned value = u;
    u = (u + 1) % N;
    return value;
  }

  T memory[N];
  unsigned head, rear, free_list[N];
};
