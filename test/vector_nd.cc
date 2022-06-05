#include "gtest/gtest.h"

#include "../containers.h"

TEST(VectorND, Constructor) {
  {
    VectorND<int, 3> v{};
    ASSERT_EQ(v.size(), 0);
  }
  {
    VectorND<int, 3> v{1, 2, 3};
    ASSERT_EQ(v.size(), 1);
    ASSERT_EQ(v[0].size(), 2);
    ASSERT_EQ(v[0][0].size(), 3);
    ASSERT_EQ(v[0][1].size(), 3);
  }
  {
    VectorND<int, 2> v{2, 3, 233};
    for (int i = 0; i < 2; ++i) {
      for (int j = 0; j < 3; ++j) {
        ASSERT_EQ(v[i][j], 233);
      }
    }
  }
}
