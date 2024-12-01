#!/bin/bash

# Please make this loop over the dir, you lazy sack'o
glslc vert.vert -o compiled/vert.vert.spv
glslc vert.frag -o compiled/vert.frag.spv
