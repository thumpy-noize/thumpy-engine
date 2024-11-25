

enable_testing()

set(TEST_SOURCES
  test_test.cc
  logger_test.cc
)
add_executable(engine_unit_test ${TEST_SOURCES})

target_link_libraries(
  engine_unit_test
  GTest::gtest_main
  logger
)

  include(GoogleTest)
gtest_discover_tests(engine_unit_test)
