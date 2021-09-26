#include <cctype>
#include <cstdint>
#include <cstdio>

struct FastIn {
  FastIn(FILE *in_ = stdin) : in(in_) {}

  char next_char() {
    if (head == length) {
      head = 0;
      length = fread(buffer, 1, LENGTH, in);
    }
    return buffer[head++];
  }

  uint64_t next_uint() {
    char c = next_char();
    while (!std::isdigit(c)) {
      c = next_char();
    }
    uint64_t x = 0;
    for (; std::isdigit(c); c = next_char()) {
      x = x * 10U + c - '0';
    }
    return x;
  }

  int64_t next_int() {
    char c = next_char();
    while (!std::isdigit(c) && c != '-') {
      c = next_char();
    }
    int64_t sign = 1;
    if (c == '-') {
      sign = -1;
      c = next_char();
    }
    int64_t x = 0;
    for (; std::isdigit(c); c = next_char()) {
      x = x * 10 + c - '0';
    }
    return sign * x;
  }

private:
  static const int LENGTH = 1 << 16;

  char buffer[LENGTH];
  size_t head = 0;
  size_t length = 0;
  FILE *in;
};

struct FastOut {
  static const int BUFSIZE = 4096;

  void putchar(char c) {
    buf[len++] = c;
    if (len == BUFSIZE) {
      fwrite(buf, 1, BUFSIZE, stdout);
      len = 0;
    }
  }

  void puts(const char *s) {
    while (*s) {
      putchar(*(s++));
    }
    putchar('\n');
  }

  void print(int64_t n) {
    static char bufn[32];
    bool negative = false;
    if (n < 0) {
      n = -n;
      negative = true;
    }
    int top = 32;
    while (n >= 10) {
      bufn[--top] = '0' + n % 10;
      n /= 10;
    }
    bufn[--top] = '0' + n;
    if (negative) {
      bufn[--top] = '-';
    }
    for (; top < 32; ++top) {
      putchar(bufn[top]);
    }
  }

  void println(int64_t n) {
    print(n);
    putchar('\n');
  }

  ~FastOut() {
    if (len) {
      fwrite(buf, 1, len, stdout);
    }
    fflush(stdout);
  }

  char buf[BUFSIZE];
  int len = 0;
};

static FastIn STDIN;
static FastOut STDOUT;
