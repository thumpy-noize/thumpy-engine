
add_library(window_manager "")

find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)



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
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_construct.hpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_device.hpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_swap_chain.hpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_pipeline.hpp
  # ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_buffers.hpp
  # ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_image.hpp
  # ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_render.hpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_helper.hpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_initializers.hpp

  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_window.cpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_debug.cpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_construct.cpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_device.cpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_swap_chain.cpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_pipeline.cpp
  # ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_buffers.cpp
  # ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_image.cpp
  # ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_render.cpp
  ${CMAKE_CURRENT_LIST_DIR}/vulkan/vulkan_helper.cpp

  # ${CMAKE_CURRENT_LIST_DIR}/vulkan/examples/vulkan_triangle_example.hpp
  # ${CMAKE_CURRENT_LIST_DIR}/vulkan/examples/vulkan_mesh_example.hpp
  
  # ${CMAKE_CURRENT_LIST_DIR}/vulkan/examples/vulkan_triangle_example.cpp
  # ${CMAKE_CURRENT_LIST_DIR}/vulkan/examples/vulkan_mesh_example.cpp

)

target_compile_definitions(window_manager 
  PUBLIC 
    VULKAN_HPP_DISPATCH_LOADER_DYNAMIC=1 
    VULKAN_HPP_NO_STRUCT_CONSTRUCTORS=1
)

target_include_directories(window_manager
  PUBLIC
    "${CMAKE_SOURCE_DIR}/include"
    ${CMAKE_CURRENT_LIST_DIR}
    ${GLFW_LIBRARIES}
  )

target_link_libraries(window_manager
  logger
  glfw
  glm::glm
  Vulkan::Vulkan
)


