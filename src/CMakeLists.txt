
set(SOURCES
  main.cpp
)

# Add executable
add_executable(engine ${SOURCES})
set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

add_subdirectory(io)

target_link_libraries(engine
  PUBLIC
  ${CURSES_LIBRARY}
  PRIVATE
    io
)
