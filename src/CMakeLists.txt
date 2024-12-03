

set(SOURCES
  main.cpp
)

find_package(fmt CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)


# Add executable
add_executable(engine ${SOURCES})

# Compile shaders
include("assets/shaders/compile_shaders.cmake")

set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(io)
add_subdirectory(logger)
add_subdirectory(window_manager)

target_link_libraries(engine
  PRIVATE
    io
    logger
    window_manager
)

# Unit testing
include(testing.cmake)






