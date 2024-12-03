
macro(compile_shaders)
    cmake_parse_arguments("MY" "TARGET" "SOURCES" ${ARGN})

    foreach(src ${MY_SOURCES})
    message("Compiling shader: ${CMAKE_CURRENT_LIST_DIR}/${src}")
        set(OUTF "${CMAKE_CURRENT_BINARY_DIR}/shaders/${src}.spv")
        get_filename_component(PARENT_DIR "${OUTF}" DIRECTORY)
        message("Output file: ${OUTF}")

        add_custom_command(
            OUTPUT "${OUTF}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PARENT_DIR}"
            COMMAND "${Vulkan_GLSLC_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/${src}" -o "${OUTF}"
            DEPENDS "${CMAKE_CURRENT_LIST_DIR}/${src}"
            VERBATIM
        )
        # src in order to get listed in the IDE, OUTF to declare the build relationship
        target_sources("engine" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/${src}" "${OUTF}")
    endforeach()
endmacro()

macro(copy_shaders)
    cmake_parse_arguments("MY" "TARGET" "SOURCES" ${ARGN})
    message("Copying shader: ${CMAKE_CURRENT_LIST_DIR}/compiled/${MY_SOURCES}.spv")
    set(OUTF "${CMAKE_CURRENT_BINARY_DIR}/shaders/${MY_SOURCES}.spv")
    message("Copying to: ${OUTF}")

    configure_file("${CMAKE_CURRENT_LIST_DIR}/compiled/${MY_SOURCES}.spv" "${OUTF}" COPYONLY)
endmacro()



if( $ENV{COMPILE_SHADERS} )
    message("Compiling shaders...")
    # Make this loop plz...
    compile_shaders(engine SOURCES "vert.vert")
    compile_shaders(engine SOURCES "uniform_buffer.vert")
    compile_shaders(engine SOURCES "vert.frag")

else()
    message("Copying shaders...")
    # This too
    copy_shaders(engine SOURCES "vert.vert")
    copy_shaders(engine SOURCES "uniform_buffer.vert")
    copy_shaders(engine SOURCES "vert.frag")

endif()



