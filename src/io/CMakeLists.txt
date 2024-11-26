
add_library(io "")

target_sources(io
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/input_manager.hpp
  ${CMAKE_CURRENT_LIST_DIR}/input_manager.cpp
)

target_include_directories(io
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

  target_link_libraries(io
    logger
  )
