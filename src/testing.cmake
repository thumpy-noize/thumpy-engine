

##################################
########## Unit Testing ##########
##################################

find_package(GTest CONFIG REQUIRED)

enable_testing()

set(TEST_SOURCES
  testing/logger_test.cc
  testing/window_manager_test.cc
)

add_executable(engine_unit_test ${TEST_SOURCES})
set_target_properties(engine_unit_test PROPERTIES LINKER_LANGUAGE CXX)

include_directories(logger)

target_link_libraries(
  engine_unit_test
  GTest::gtest_main
  logger
  window_manager
)

include(GoogleTest)
gtest_discover_tests(engine_unit_test)