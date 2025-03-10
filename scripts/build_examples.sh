#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build
cd build
rm -rf *

# Configure with CMake
cmake -DBUILD_EXAMPLES=ON -DENABLE_TESTS=Off -DPROFILING=Off -DCMAKE_BUILD_TYPE=Debug ..

# Build
make

# Run passed apps as $@ argument
# Example: ./build.sh SimpleTest LoopHookTest
for app in "$@"; do
	./build/${app}
done
	    
