#!/bin/bash

# Remove Build
[[ -d build ]] && rm -rf build

echo "Clang Formating..."
clang-format -i -style=WebKit *.cpp *.hpp

echo "Creating Build..."
mkdir build
cd build
cmake ../
cmake --build .


echo "Executing Build..."
./src/engine
