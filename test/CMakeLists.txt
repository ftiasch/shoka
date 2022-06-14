add_executable(debug debug.cc)
target_link_libraries(debug PRIVATE gtest_main)
gtest_add_tests(TARGET debug)

add_executable(montgomery montgomery.cc)
target_link_libraries(montgomery PRIVATE gtest_main)
gtest_add_tests(TARGET montgomery)

add_executable(nd_vector nd_vector.cc)
target_link_libraries(nd_vector PRIVATE gtest_main)
gtest_add_tests(TARGET nd_vector)

# add_executable(ntt ntt.cc)
# target_link_libraries(ntt PRIVATE gtest_main)
# gtest_add_tests(TARGET ntt)

add_executable(ntt_v1 ntt_v1.cc)
target_link_libraries(ntt_v1 PRIVATE gtest_main)
gtest_add_tests(TARGET ntt_v1)

add_executable(polynomial_interpolation polynomial_interpolation.cc)
target_link_libraries(polynomial_interpolation PRIVATE gtest_main)
gtest_add_tests(TARGET polynomial_interpolation)

add_executable(multi_eval multi_eval.cc)
target_link_libraries(multi_eval PRIVATE gtest_main)
gtest_add_tests(TARGET multi_eval)

add_executable(segment_tree segment_tree.cc)
target_link_libraries(segment_tree PRIVATE gtest_main)
gtest_add_tests(TARGET segment_tree)

add_executable(rmq rmq.cc)
target_link_libraries(rmq PRIVATE gtest_main)
gtest_add_tests(TARGET rmq)

add_executable(universal_euclidean universal_euclidean.cc)
target_link_libraries(universal_euclidean PRIVATE gtest_main)
gtest_add_tests(TARGET universal_euclidean)

add_executable(y_combinator y_combinator.cc)
target_link_libraries(y_combinator PRIVATE gtest_main)
gtest_add_tests(TARGET y_combinator)
