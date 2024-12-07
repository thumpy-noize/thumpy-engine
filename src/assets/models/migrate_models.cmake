
message("Copying models...")
set(srcDir ${CMAKE_CURRENT_LIST_DIR})
set(destDir ${CMAKE_CURRENT_BINARY_DIR}/assets/models)

message("Coping textures from: ${srcDir}")
message("Coping textures to: ${destDir}")
make_directory(${destDir})
file( GLOB_RECURSE  models "${srcDir}/*.obj" )

foreach(model ${models})

    file(RELATIVE_PATH relative_path ${srcDir} ${model})

    message("Copying model: ${model}")
    message("Copying to: ${destDir}/${relative_path}")
    configure_file(${model} "${destDir}/${relative_path}" COPYONLY)

endforeach(model ${models})





