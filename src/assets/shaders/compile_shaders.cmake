

set(srcDir ${CMAKE_CURRENT_LIST_DIR})
set(destDir ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders)

make_directory(${destDir})

# Get all files with the following extentions
# vert
# frag

# Compile shaders
if( $ENV{COMPILE_SHADERS} )
    message("Compiling shaders...")
    file( GLOB_RECURSE  shaders "${srcDir}/*.vert" "${srcDir}/*.frag")

    foreach(shader ${shaders})
        file(RELATIVE_PATH relative_path ${srcDir} ${shader})

        message("Compiling shader: ${CMAKE_CURRENT_LIST_DIR}/${relative_path}")
        set(OUTF "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/${relative_path}.spv")
        get_filename_component(PARENT_DIR "${OUTF}" DIRECTORY)
        message("Output file: ${OUTF}")

        # Compile with glslc
        add_custom_command(
            OUTPUT "${OUTF}"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${PARENT_DIR}"
            COMMAND "${Vulkan_GLSLC_EXECUTABLE}" "${CMAKE_CURRENT_LIST_DIR}/${relative_path}" -o "${OUTF}"
            DEPENDS "${CMAKE_CURRENT_LIST_DIR}/${relative_path}"
            VERBATIM
        )

        target_sources("engine" PRIVATE "${CMAKE_CURRENT_LIST_DIR}/${relative_path}" "${OUTF}")

    endforeach(shader ${shaders})


else() # Copy shaders
    message("Copying shaders...")
    file( GLOB_RECURSE shaders "${srcDir}/*.spv")

    foreach(shader ${shaders})
        file(RELATIVE_PATH relative_path "${srcDir}/compiled" ${shader})
        message("Copying shader: ${shader}")
        message("Copying to: ${destDir}/${relative_path}")
        configure_file("${shader}" "${destDir}/${relative_path}" COPYONLY)
    endforeach(shader ${shaders})

endif()



