#include <filesystem>
#include <limits>

#include "gtest/gtest.h"

#include "../fast_io.h"

TEST(IO, FastIO) {
  auto tmpf = std::tmpfile();
  std::fprintf(tmpf, "%d 0 %d\n", std::numeric_limits<int>::min(),
               std::numeric_limits<int>::max());
  std::fprintf(tmpf, "%lld 0 %lld\n", std::numeric_limits<long long>::min(),
               std::numeric_limits<long long>::max());
  std::rewind(tmpf);
  FastIO io{tmpf};
  ASSERT_EQ(io.read1(), std::numeric_limits<int>::min());
  ASSERT_EQ(io.read1(), 0);
  ASSERT_EQ(io.read1(), std::numeric_limits<int>::max());
  ASSERT_EQ(io.read1<long long>(), std::numeric_limits<long long>::min());
  ASSERT_EQ(io.read1<long long>(), 0);
  ASSERT_EQ(io.read1<long long>(), std::numeric_limits<long long>::max());
  std::fclose(tmpf);
}
