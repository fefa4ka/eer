cmake_minimum_required(VERSION 3.12)
project(eer_framework C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Configuration options
option(PLATFORM "Target platform (simulation or native)" simulation)

# Include directories
include_directories(include)

# Third-party dependencies
set(Criterion_DIR "/opt/homebrew/Cellar/criterion/2.4.2_2")
find_package(Criterion REQUIRED)

# Test executable
add_executable(simulator
  test/main.c
  test/components/MyComponent_test.c
  src/eer.c
)

target_link_libraries(simulator PRIVATE Criterion::criterion)

# Installation (if needed)
install(TARGETS simulator DESTINATION bin)
