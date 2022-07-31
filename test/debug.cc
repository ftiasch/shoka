#include "../debug.h"

#include "gtest/gtest.h"

#include <bits/stdc++.h>

struct Foo {
  int f;
};

std::ostream &operator<<(std::ostream &out, const Foo &f) {
  return out << "Foo(" << f.f << ")";
}

template <typename T> void test(const T &&value, const std::string &expect) {
  std::stringstream out;
  out << value;
  ASSERT_EQ(out.str(), expect);
}

TEST(Debug, Pair) {
  test(std::pair<Foo, bool>{Foo{233}, true}, "(Foo(233), 1)");
}

TEST(Debug, Tuple) {
  test(std::tuple<Foo, char, std::string>{Foo{233}, 'a', "test"},
       "(Foo(233), a, test)");
}

TEST(Debug, Array) {
  test(std::array<Foo, 3>{Foo{2}, Foo{3}, Foo{5}}, "[Foo(2), Foo(3), Foo(5)]");
}

TEST(Debug, Array2D) {
  test(std::array<std::array<Foo, 2>, 2>{std::array<Foo, 2>{Foo{2}, Foo{3}},
                                         {Foo{5}, Foo{6}}},
       "[[Foo(2), Foo(3)], [Foo(5), Foo(6)]]");
}

TEST(Debug, Vector) {
  test(std::vector<Foo>{Foo{2}, Foo{3}, Foo{5}}, "[Foo(2), Foo(3), Foo(5)]");
}

TEST(Debug, Vector2D) {
  test(std::vector<std::vector<int>>{{2, 3}, {4, 5, 6}}, "[[2, 3], [4, 5, 6]]");
}

TEST(Debug, VectorArray) {
  test(std::vector<std::array<int, 2>>{{2, 3}, {4, 5}}, "[[2, 3], [4, 5]]");
}

TEST(Debug, Map) {
  test(std::map<int, Foo>{{2, Foo{3}}, {3, Foo{5}}}, "{2: Foo(3), 3: Foo(5)}");
}
