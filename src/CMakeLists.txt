
set(SOURCES
  main.cpp
)

# Add executable
add_executable(engine ${SOURCES})
set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(io)
add_subdirectory(logger)

target_link_libraries(engine
  PRIVATE
    io
    logger
)
