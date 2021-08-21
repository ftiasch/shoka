#include <cstdint>
#include <cstdlib>
#include <numeric>

template <int SIZE> struct FreeQueue {
  FreeQueue() { std::iota(queue, queue + SIZE, 0); }

  int allocate() {
    int index = queue[head++];
    if (head == SIZE) {
      head = 0;
    }
    return index;
  }

  void free(int index) {
    queue[rear++] = index;
    if (rear == SIZE) {
      rear = 0;
    }
  }

private:
  int head = 0, rear = 0, queue[SIZE];
};
