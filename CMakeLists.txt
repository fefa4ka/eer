cmake_minimum_required(VERSION 3.12)
project(eer_framework C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(EER_VERSION "0.1.0")
option(PROFILING "Enable profiling" OFF)
option(ENABLE_TESTS "Enable building of tests" OFF)
option(BUILD_EXAMPLES "Build example applications" ON)

# Configuration options
option(PLATFORM "Target platform (simulation or native)" simulation)

# Third-party dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Add sources
add_library(eer src/eer.c)
target_compile_definitions(eer PUBLIC EER_VERSION="${EER_VERSION}")
target_include_directories(eer PUBLIC include)
set_property(TARGET eer PROPERTY C_STANDARD 99)

if(PROFILING)
  message("Profiling enabled")
  add_library(profiling STATIC profiler/profiler.c profiler/hash.c
                               profiler/log.c)
  target_include_directories(profiling PUBLIC profiler include)
  target_link_libraries(eer profiling)
  target_compile_definitions(profiling PUBLIC PROFILING)
elseif(ENABLE_TESTS)
  add_library(logging STATIC profiler/log.c)
  target_include_directories(logging PUBLIC profiler)
  target_compile_definitions(logging PUBLIC LOGGING)
  target_link_libraries(eer logging)
endif()

if(ENABLE_TESTS)
  # Add tests
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/preprocessor.cmake)
  enable_testing()

  # Add test utilities
  add_library(test_utils STATIC test/test_utils.c)
  target_include_directories(test_utils PUBLIC include test)

  # List of test source files
  set(TEST_SOURCES test/MyComponent.c test/SimpleTest.c test/LoopHookTest.c)

  # Apply the preprocessor to each test source file
  apply_preprocessor_to_sources(SOURCES ${TEST_SOURCES} TARGET eer)
  add_custom_target(GenerateSources ALL DEPENDS ${GENERATED_FILES})
  # Create executables from preprocessed files
  foreach(SOURCE ${TEST_SOURCES})
    get_filename_component(SOURCE_NAME ${SOURCE} NAME_WE)
    add_executable(${SOURCE_NAME} ${CMAKE_BINARY_DIR}/${SOURCE_NAME}.e.c)
    add_dependencies(${SOURCE_NAME} GenerateSources)
    target_include_directories(${SOURCE_NAME} PUBLIC include test)
    target_link_libraries(${SOURCE_NAME} eer test_utils)
    add_test(NAME ${SOURCE_NAME} COMMAND ${SOURCE_NAME})
  endforeach()
endif()

# Build examples
if(BUILD_EXAMPLES)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/preprocessor.cmake)

  # List of example source files
  set(EXAMPLE_SOURCES examples/BasicExample.c examples/LifecycleExample.c)

  # Apply the preprocessor to each example source file
  apply_preprocessor_to_sources(SOURCES ${EXAMPLE_SOURCES} TARGET eer)
  add_custom_target(ExamplesSources ALL DEPENDS ${GENERATED_FILES})

  # Create executables from preprocessed files
  foreach(SOURCE ${EXAMPLE_SOURCES})
    get_filename_component(SOURCE_NAME ${SOURCE} NAME_WE)
    add_executable(${SOURCE_NAME} ${CMAKE_BINARY_DIR}/${SOURCE_NAME}.e.c)
    add_dependencies(${SOURCE_NAME} ExamplesSources)
    target_include_directories(${SOURCE_NAME} PUBLIC include)

    target_link_libraries(${SOURCE_NAME} eer)
  endforeach()
endif()
