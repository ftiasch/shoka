#include <cstdint>
#include <cstdlib>
#include <numeric>
#include <vector>

struct FreeQueue {
  explicit FreeQueue(int size_) : size(size_), head(0), rear(0), queue(size) {
    std::iota(queue.begin(), queue.end(), 0);
  }

  int allocate() {
    int index = queue[head++];
    if (head == size) {
      head = 0;
    }
    return index;
  }

  void free(int index) {
    queue[rear++] = index;
    if (rear == size) {
      rear = 0;
    }
  }

private:
  int size, head, rear;
  std::vector<int> queue;
};
