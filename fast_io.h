#include "io_base.h"

#include <cctype>
#include <cstdint>
#include <cstdio>

struct FastIO : public IOBaseT<FastIO> {
  friend class IOBaseT<FastIO>;

  explicit FastIO(std::FILE *f_ = stdin) : f{f_} {}

private:
  static const size_t BUF_SIZE = 1 << 16;

  char getc() {
    if (head == rear) {
      head = 0;
      rear = fread(buf, 1, BUF_SIZE, f);
    }
    return head < rear ? buf[head++] : '\0';
  }

  template <typename T> void read1(T &&v) {
    if constexpr (std::is_same_v<std::decay_t<T>, char>) {
      v = getc();
    } else {
      if constexpr (std::is_integral_v<std::decay_t<T>>) {
        char c = getc();
        while (!std::isdigit(c) && c != '-') {
          c = getc();
        }
        bool is_neg = false;
        if (c == '-') {
          is_neg = true;
          c = getc();
        }
        std::decay_t<T> r = 0;
        for (; std::isdigit(c); c = getc()) {
          r = r * 10 + (c - '0');
        }
        v = is_neg ? -r : r;
      } else {
        static_assert(!sizeof(T *));
      }
    }
  }

  std::FILE *f;

  char buf[BUF_SIZE];
  size_t head = 0, rear = 0;
};
