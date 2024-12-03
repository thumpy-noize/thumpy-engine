

set(srcDir ${CMAKE_CURRENT_LIST_DIR})
set(destDir ${CMAKE_CURRENT_BINARY_DIR}/assets/shaders)

make_directory(${destDir})

# Get all files with the following extentions
# vert
# frag
file( GLOB_RECURSE  textures "${srcDir}/*.vert" "${srcDir}/*.frag")

# Compile shaders
if( true ) #$ENV{COMPILE_SHADERS} )
    message("Compiling shaders...")
    foreach(texture ${textures})
        file(RELATIVE_PATH relative_path ${srcDir} ${texture})

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

    endforeach(texture ${textures})


else() # Copy shaders
    message("Copying shaders...")

    foreach(texture ${textures})
        file(RELATIVE_PATH relative_path ${srcDir} ${texture})
        message("Copying shader: ${texture}")
        message("Copying to: ${destDir}/${relative_path}.spv")
        configure_file(${texture} "${destDir}/${relative_path.spv}" COPYONLY)
    endforeach(texture ${textures})

endif()



