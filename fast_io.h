#include <cctype>
#include <cstdint>
#include <cstdio>
#include <type_traits>

#include <iostream>

struct FastIO {
  explicit FastIO(std::FILE *f_ = stdin) : f{f_} {}

  template <typename T = int> auto read1() {
    if constexpr (std::is_same_v<T, char>) {
      return getc();
    } else {
      if constexpr (std::is_integral_v<T>) {
        char c = getc();
        while (!std::isdigit(c) && c != '-') {
          c = getc();
        }
        bool is_neg = false;
        if (c == '-') {
          is_neg = true;
          c = getc();
        }
        T r = 0;
        for (; std::isdigit(c); c = getc()) {
          r = r * 10 + (c - '0');
        }
        return is_neg ? -r : r;
      } else {
        static_assert(!sizeof(T *));
      }
    }
  }

private:
  static const size_t BUF_SIZE = 1 << 16;

  char getc() {
    if (head == rear) {
      head = 0;
      rear = fread(buf, 1, BUF_SIZE, f);
    }
    return head < rear ? buf[head++] : '\0';
  }

  std::FILE *f;

  char buf[BUF_SIZE];
  size_t head = 0, rear = 0;
};
