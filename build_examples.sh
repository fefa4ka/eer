#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build
cd build

# Configure with CMake
cmake -DBUILD_EXAMPLES=ON -DCMAKE_BUILD_TYPE=Debug ..

# Build
make

# Show success message
echo -e "\n\033[1;32mExamples built successfully!\033[0m"
echo "Run the basic example with: ./BasicExample"
