


macro(migrate_texture)
    cmake_parse_arguments("MY" "TARGET" "SOURCES" ${ARGN})
    message("Copying shader: ${CMAKE_CURRENT_LIST_DIR}/compiled/${MY_SOURCES}.spv")
    set(OUTF "${CMAKE_CURRENT_BINARY_DIR}/assets/shaders/${MY_SOURCES}.spv")
    message("Copying to: ${OUTF}")

    configure_file("${CMAKE_CURRENT_LIST_DIR}/compiled/${MY_SOURCES}.spv" "${OUTF}" COPYONLY)
endmacro()


macro(configure_files srcDir destDir)
    message("Coping textures from: ${srcDir}")
    message("Coping textures to: ${destDir}")
    make_directory(${destDir})
    file( GLOB_RECURSE  textures "${srcDir}/*.png" "${srcDir}/*.jpg")

    foreach(texture ${textures})

    file(RELATIVE_PATH relative_path ${srcDir} ${texture})

        message("Copying texture: ${texture}")
        message("Copying to: ${destDir}/${relative_path}")
        configure_file(${texture} "${destDir}/${relative_path}" COPYONLY)

    endforeach(texture ${textures})
    
endmacro(configure_files)




message("Copying textures...")
# configure_files(${CMAKE_CURRENT_LIST_DIR} "${CMAKE_CURRENT_BINARY_DIR}/assets/textures")
set(srcDir ${CMAKE_CURRENT_LIST_DIR})
set(destDir ${CMAKE_CURRENT_BINARY_DIR}/assets/textures)

message("Coping textures from: ${srcDir}")
message("Coping textures to: ${destDir}")
make_directory(${destDir})
file( GLOB_RECURSE  textures "${srcDir}/*.png" "${srcDir}/*.jpg")

foreach(texture ${textures})

file(RELATIVE_PATH relative_path ${srcDir} ${texture})

    message("Copying texture: ${texture}")
    message("Copying to: ${destDir}/${relative_path}")
    configure_file(${texture} "${destDir}/${relative_path}" COPYONLY)

endforeach(texture ${textures})





