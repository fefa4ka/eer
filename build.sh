#!/bin/bash

# Clean build directory
rm -rf *

# Configure with CMake
cmake -DENABLE_TESTS=ON -DPROFILING=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ../

# Build
make

# Show success message
echo -e "\n\033[1;32mBuild completed successfully!\033[0m"
echo "Run tests with: ctest"
echo "Or run individual tests like: ./SimpleTest"
