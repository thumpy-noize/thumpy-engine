
message("Copying textures...")
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





