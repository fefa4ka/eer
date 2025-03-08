cmake_minimum_required(VERSION 3.12)
project(eer_framework C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Configuration options
option(PLATFORM "Target platform (simulation or native)" simulation)

# Third-party dependencies
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include directories
include_directories(include)

# Add sources
add_subdirectory (src)

# if (ENABLE_TESTS)
  # Add tests
  enable_testing ()
  add_subdirectory (test)
# endif ()
