#include "debug.h"

#include <bits/stdc++.h>

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
  auto test_helper = [&](auto &&value, const std::string &expected) {
    std::stringstream out;
    out << value;
    REQUIRE(out.str() == expected);
  };

  using namespace debug;

  SECTION("std::pair") {
    test_helper(std::pair<Foo, bool>{Foo{233}, true}, "(Foo(233), 1)");
  }

  SECTION("std::tuple") {
    test_helper(std::tuple<Foo, char, std::string>{Foo{233}, 'a', "test"},
                "(Foo(233), a, test)");
  }

  SECTION("const char*") {
    const char* s = "shoka";
    test_helper(s, "shoka");
  }

  SECTION("std::string") {
    test_helper(std::string{"shoka"}, "shoka");
  }

  SECTION("std::basic_string") {
    test_helper(std::basic_string<Foo>{Foo{2}, Foo{3}, Foo{5}}, "[Foo(2), Foo(3), Foo(5)]");
  }

  SECTION("std::array") {
    test_helper(std::array<Foo, 3>{Foo{2}, Foo{3}, Foo{5}},
                "[Foo(2), Foo(3), Foo(5)]");
  }

  SECTION("std::array<std::array>") {
    test_helper(
        std::array<std::array<Foo, 2>, 2>{std::array<Foo, 2>{Foo{2}, Foo{3}},
                                          {Foo{5}, Foo{6}}},
        "[[Foo(2), Foo(3)], [Foo(5), Foo(6)]]");
  }

  SECTION("std::vector") {
    test_helper(std::vector<Foo>{Foo{2}, Foo{3}, Foo{5}},
                "[Foo(2), Foo(3), Foo(5)]");
  }

  SECTION("std::vector<std::array>") {
    test_helper(std::vector<std::array<int, 2>>{{2, 3}, {4, 5}},
                "[[2, 3], [4, 5]]");
  }

  SECTION("std::vector<std::vector>") {
    test_helper(std::vector<std::vector<int>>{{2, 3}, {4, 5, 6}},
                "[[2, 3], [4, 5, 6]]");
  }

  SECTION("std::map") {
    test_helper(std::map<int, Foo>{{2, Foo{3}}, {3, Foo{5}}},
                "[(2, Foo(3)), (3, Foo(5))]");
  }
}
