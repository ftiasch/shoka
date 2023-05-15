add_executable(tests tests.cc)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR})
catch_discover_tests(tests)

add_executable(benchmark benchmark.cc)
target_link_libraries(benchmark PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(benchmark)
