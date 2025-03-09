cmake_minimum_required(VERSION 3.12)
project(eer_framework C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
option(PROFILING "Enable profiling" OFF)

# Configuration options
option(PLATFORM "Target platform (simulation or native)" simulation)

# Third-party dependencies
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Include directories
include_directories(include /usr/include /usr/local/include)

# Add sources
add_library (eer src/eer.c)
set_property(TARGET eer PROPERTY C_STANDARD 99)

if(PROFILING)
    message("Profiling enabled")
    add_library(profiling STATIC profiler/profiler.c profiler/hash.c profiler/log.c)
    target_include_directories(profiling PUBLIC profiler)
    target_link_libraries(eer profiling)
    target_compile_definitions(profiling PUBLIC PROFILING)
endif()
                      

# if (ENABLE_TESTS)
  # Add tests
  enable_testing ()
  add_subdirectory (test)

# endif ()
