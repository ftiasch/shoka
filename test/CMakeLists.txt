add_executable(debug debug.cc)
target_link_libraries(debug PRIVATE gtest_main)
gtest_add_tests(TARGET debug)

add_executable(montgomery montgomery.cc)
target_link_libraries(montgomery PRIVATE gtest_main)
gtest_add_tests(TARGET montgomery)

add_executable(ntt ntt.cc)
target_link_libraries(ntt PRIVATE gtest_main)
gtest_add_tests(TARGET ntt)

add_executable(segment_tree segment_tree.cc)
target_link_libraries(segment_tree PRIVATE gtest_main)
gtest_add_tests(TARGET segment_tree)

add_executable(rmq rmq.cc)
target_link_libraries(rmq PRIVATE gtest_main)
gtest_add_tests(TARGET rmq)

add_executable(universal_euclidean universal_euclidean.cc)
target_link_libraries(universal_euclidean PRIVATE gtest_main)
gtest_add_tests(TARGET universal_euclidean)
