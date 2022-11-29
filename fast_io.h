#include <cctype>
#include <cstdint>
#include <cstdio>
#include <type_traits>

struct FastIO {
  explicit FastIO(std::FILE *f_ = stdin) : f{f_} {}

  template <typename T = int> T read1();

  char getc() {
    if (head == rear) {
      head = 0;
      rear = fread(buf, 1, BUF_SIZE, f);
    }
    return buf[head++];
  }

private:
  static const size_t BUF_SIZE = 1 << 16;

  std::FILE *f;

  char buf[BUF_SIZE];
  size_t head = 0, rear = 0;
};

namespace fast_io {

template <typename T>
typename std::enable_if_t<std::is_integral_v<T>, T> read1(FastIO &io) {
  char c = io.getc();
  while (!std::isdigit(c) && c != '-') {
    c = io.getc();
  }
  bool is_neg = false;
  if (c == '-') {
    is_neg = true;
    c = io.getc();
  }
  T r = 0;
  for (; std::isdigit(c); c = io.getc()) {
    r = r * 10 + c - '0';
  }
  return is_neg ? -r : r;
}

template <typename T>
std::enable_if_t<std::is_same_v<T, char>, T> read1(FastIO &io) {
  return io.getc();
}

} // namespace fast_io

template <typename T> T FastIO::read1() { return fast_io::read1<T>(*this); }
