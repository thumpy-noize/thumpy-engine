


# macro(add_shaders)
#     cmake_parse_arguments("MY" "TARGET" "SOURCES" ${ARGN})

#     message("Adding shader")

#     message("${MY_SOURCES}")


#     foreach(src ${MY_SOURCES})
#     message("Compiling shader: ${src}")
#         set(OUTF "${CMAKE_CURRENT_BINARY_DIR}/${src}.spv")
#         get_filename_component(PARENT_DIR "${OUTF}" DIRECTORY)
#         message("${OUTF}")

#         add_custom_command(
#             OUTPUT "${OUTF}"
#             COMMAND ${CMAKE_COMMAND} -E make_directory "${PARENT_DIR}"
#             COMMAND "${Vulkan_GLSLC_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/${src}" -o "${OUTF}"
#             DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${src}"
#             VERBATIM
#         )
#         # src in order to get listed in the IDE, OUTF to declare the build relationship
#         target_sources("engine" PRIVATE "${src}" "${OUTF}")
#     endforeach()
# endmacro()




set(SOURCES
  main.cpp
)

find_package(fmt CONFIG REQUIRED)
find_package(GTest CONFIG REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Vulkan REQUIRED)

# Add executable
add_executable(engine ${SOURCES})

# Compile shaders
include("shaders/compile_shaders.cmake")

set_target_properties(engine PROPERTIES LINKER_LANGUAGE CXX)

add_subdirectory(io)
add_subdirectory(logger)
add_subdirectory(window_manager)

target_link_libraries(engine
  PRIVATE
    io
    logger
    window_manager

    glfw
    vulkan
    glm::glm

    ${GLFW_LIBRARIES}
)

##################################
########## Unit Testing ##########
##################################

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
