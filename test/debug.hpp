#include "debug.h"
#include "fixed_size_matrix.h"

#include <bits/stdc++.h>

using namespace std::string_view_literals;

#include <catch2/catch_all.hpp>

namespace debug {

struct Foo {
  int f;
};

} // namespace debug

namespace std {

ostream &operator<<(ostream &out, const debug::Foo &f) {
  return out << "Foo(" << f.f << ")";
}

} // namespace std

TEST_CASE("debug") {
  auto test_helper = [&](auto &&value, std::string_view expected) {
    std::stringstream out;
    out << value;
    REQUIRE(out.str() == expected);
  };

  using namespace debug;

  SECTION("std::pair") {
    test_helper(std::pair<Foo, bool>{Foo{233}, true}, "(Foo(233), 1)"sv);
  }

  SECTION("std::tuple") {
    test_helper(std::tuple<Foo, char, std::string>{Foo{233}, 'a', "test"},
                "(Foo(233), a, test)"sv);
  }

  SECTION("const char*") {
    const char *s = "shoka";
    test_helper(s, "shoka"sv);
  }

  SECTION("std::string") { test_helper(std::string{"shoka"}, "shoka"sv); }

  SECTION("Binary") {
    test_helper(Binary{233, 10}, "(1001011100)_2"sv);
    test_helper(Binary<int>{233}, "(1001011100000000000000000000000)_2"sv);
    test_helper(
        Binary<uint64_t>{233},
        "(1001011100000000000000000000000000000000000000000000000000000000)_2"sv);
  }

#if (__cplusplus >= 202002L)
  SECTION("std::basic_string") {
    test_helper(std::basic_string<Foo>{Foo{2}, Foo{3}, Foo{5}},
                "[Foo(2), Foo(3), Foo(5)]"sv);
  }

  SECTION("std::array") {
    test_helper(std::array<Foo, 3>{Foo{2}, Foo{3}, Foo{5}},
                "[Foo(2), Foo(3), Foo(5)]"sv);
  }

  SECTION("std::array<std::array>") {
    test_helper(
        std::array<std::array<Foo, 2>, 2>{std::array<Foo, 2>{Foo{2}, Foo{3}},
                                          {Foo{5}, Foo{6}}},
        "[[Foo(2), Foo(3)], [Foo(5), Foo(6)]]"sv);
  }

  SECTION("std::vector") {
    test_helper(std::vector<Foo>{Foo{2}, Foo{3}, Foo{5}},
                "[Foo(2), Foo(3), Foo(5)]"sv);
  }

  SECTION("std::vector<std::array>") {
    test_helper(std::vector<std::array<int, 2>>{{2, 3}, {4, 5}},
                "[[2, 3], [4, 5]]"sv);
  }

  SECTION("std::vector<std::vector>") {
    test_helper(std::vector<std::vector<int>>{{2, 3}, {4, 5, 6}},
                "[[2, 3], [4, 5, 6]]"sv);
  }

  SECTION("std::map") {
    test_helper(std::map<int, Foo>{{2, Foo{3}}, {3, Foo{5}}},
                "[(2, Foo(3)), (3, Foo(5))]"sv);
  }

  SECTION("FixedSizeMatrixT") {
    using Matrix = FixedSizeMatrixT<int, 2>;
    test_helper(Matrix{{2, 3}, {4, 5}}, "[[2, 3], [4, 5]]");
  }
#endif
}
