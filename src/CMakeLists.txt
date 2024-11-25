
set(SOURCES
  main.cpp
)

# Add executable
add_executable(engine ${SOURCES})


set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(io)
add_subdirectory(logger)
# add_subdirectory(testing)

target_link_libraries(engine
  PRIVATE
    io
    logger

    ${GLFW_LIBRARIES}
)

# Unit Testing

enable_testing()

set(TEST_SOURCES
  testing/logger_test.cc
)
add_executable(engine_unit_test ${TEST_SOURCES})

set_target_properties(engine_unit_test PROPERTIES LINKER_LANGUAGE CXX)

include_directories(logger)

target_link_libraries(
  engine_unit_test
  GTest::gtest_main
  logger
)

  include(GoogleTest)
gtest_discover_tests(engine_unit_test)
