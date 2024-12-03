#!/bin/bash

# Please make this loop over the dir, you lazy sack'o

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
