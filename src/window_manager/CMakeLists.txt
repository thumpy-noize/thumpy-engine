
add_library(window_manager "")

target_sources(window_manager
  PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}/window_manager.hpp
  ${CMAKE_CURRENT_LIST_DIR}/window_manager.cpp
  ${CMAKE_CURRENT_LIST_DIR}/window.hpp
  ${CMAKE_CURRENT_LIST_DIR}/window.cpp

  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_window.hpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_window.cpp
)

target_include_directories(window_manager
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

  target_link_libraries(window_manager
    logger
  )
