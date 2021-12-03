#include "../debug.h"

#include "gtest/gtest.h"

TEST(Debug, Pair) {
  std::pair<int, bool> p{233, true};
  std::stringstream out;
  out << p;
  ASSERT_EQ(out.str(), "(233, 1)");
}

TEST(Debug, Tuple) {
  std::tuple<int, char, std::string> p{233, 'a', "test"};
  std::stringstream out;
  out << p;
  ASSERT_EQ(out.str(), "(233, a, test)");
}

TEST(Debug, Vector) {
  std::vector<int> v{2, 3, 5};
  std::stringstream out;
  out << v;
  ASSERT_EQ(out.str(), "[2, 3, 5]");
}
