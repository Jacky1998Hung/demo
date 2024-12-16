#!/bin/bash

# Navigate to the build directory
cd build

# Configure the project with CMake
cmake ..

# Build the project with make
make

opt --load-pass-plugin=SimpleLoopRotate.so --passes="function(simple-loop-rotate)" -S -o output.ll test.ll
