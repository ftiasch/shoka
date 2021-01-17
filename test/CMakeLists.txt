add_executable(montgomery montgomery.cc)
target_link_libraries(montgomery PRIVATE gtest_main)
gtest_add_tests(TARGET montgomery)

add_executable(ntt ntt.cc)
target_link_libraries(ntt PRIVATE gtest_main)
gtest_add_tests(TARGET ntt)