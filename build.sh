#!/bin/bash

# Clean build directory
mkdir -p build
cd build
rm -rf *

# Configure with CMake
cmake -DENABLE_TESTS=ON -DPROFILING=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ../

# Build
make

# Run passed tests as $@ argument
# Example: ./build.sh SimpleTest LoopHookTest
for test in "$@"; do
	./${test}
	cat ./profiler.log
done
	
	
