#include "io_base.h"

#include <cctype>
#include <cstdio>

struct FastIO : public IOBaseT<FastIO> {
  friend class IOBaseT<FastIO>;

  explicit FastIO(std::FILE *inf_ = stdin, std::FILE *ouf_ = stdout)
      : inf{inf_}, ouf{ouf_} {}

  ~FastIO() {
    if (orear) {
      fwrite(obuf, 1, orear, ouf);
    }
  }

private:
  static const size_t BUF_SIZE = 1 << 16;

  char getc() {
    if (ihead == irear) {
      ihead = 0;
      irear = fread(ibuf, 1, BUF_SIZE, inf);
    }
    return ihead < irear ? ibuf[ihead++] : '\0';
  }

  void putc(char c) {
    if (orear == BUF_SIZE) {
      fwrite(obuf, 1, BUF_SIZE, ouf);
      orear = 0;
    }
    obuf[orear++] = c;
  }

  template <typename T> void read1(T &&v) {
    using DecayedT = std::decay_t<T>;
    if constexpr (std::is_same_v<DecayedT, char>) {
      v = getc();
    } else if constexpr (std::is_integral_v<DecayedT>) {
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

  template <typename T> void write1(T &&v) {
    using DecayedT = std::decay_t<T>;
    if constexpr (std::is_same_v<DecayedT, char>) {
      putc(v);
    } else if constexpr (std::is_integral_v<DecayedT>) {
      sprintf(text, "%lld", static_cast<long long>(v));
      for (int i = 0; text[i]; i++) {
        putc(text[i]);
      }
    } else if constexpr (std::is_same_v<DecayedT, char *> ||
                         std::is_same_v<DecayedT, const char *>) {
      for (int i = 0; v[i]; i++) {
        putc(v[i]);
      }
    } else {
      static_assert(!sizeof(T *));
    }
  }

  std::FILE *inf, *ouf;

  char ibuf[BUF_SIZE], obuf[BUF_SIZE], text[32];
  size_t ihead = 0, irear = 0, orear = 0;
};
