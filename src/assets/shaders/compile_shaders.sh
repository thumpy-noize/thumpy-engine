#!/bin/bash

# Compile vert files
for filename in *.vert; 
    do echo "Compiling ${filename}";
    glslc ${filename} -o compiled/${filename}.spv
done


# Compile frag files
for filename in *.frag; 
    do echo "Compiling ${filename}";
    glslc ${filename} -o compiled/${filename}.spv
done


# Compile slang files
for filename in *.slang; 
    do echo "Compiling ${filename}";
    slangc  ${filename} -target spirv -profile spirv_1_4 -emit-spirv-directly -fvk-use-entrypoint-name -entry vertMain -entry fragMain -o compiled/${filename}.spv
done