
add_library(window_manager "")

target_sources(window_manager
  PUBLIC

  # base
  ${CMAKE_CURRENT_LIST_DIR}/window_manager.hpp
  ${CMAKE_CURRENT_LIST_DIR}/window_manager.cpp
  ${CMAKE_CURRENT_LIST_DIR}/window.hpp
  ${CMAKE_CURRENT_LIST_DIR}/window.cpp

  # vulkan
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_window.hpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_debug.hpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_device.hpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_swap_chain.hpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_helper.hpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_initializers.hpp

  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_window.cpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_debug.cpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_device.cpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_swap_chain.cpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_helper.cpp

)

target_include_directories(window_manager
  PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
  )

  target_link_libraries(window_manager
    logger
  )
