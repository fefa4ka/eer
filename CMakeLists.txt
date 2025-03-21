cmake_minimum_required(VERSION 3.12)
project(eer_framework C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(EER_VERSION_MAJOR 0)
set(EER_VERSION_MINOR 2)
set(EER_VERSION_PATCH 0)
set(EER_VERSION
    "${EER_VERSION_MAJOR}.${EER_VERSION_MINOR}.${EER_VERSION_PATCH}")
option(PROFILING "Enable profiler" OFF)
option(ENABLE_TESTS "Enable building of tests" OFF)
option(BUILD_EXAMPLES "Build example applications" OFF)

# Configuration options
option(PLATFORM "Target platform (simulation or native)" simulation)

# Third-party dependencies
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Add sources
add_library(eer src/eer.c)
target_compile_definitions(
  eer
  PUBLIC EER_VERSION="${EER_VERSION}" EER_VERSION_MAJOR=${EER_VERSION_MAJOR}
         EER_VERSION_MINOR=${EER_VERSION_MINOR}
         EER_VERSION_PATCH=${EER_VERSION_PATCH})
target_include_directories(eer PUBLIC include)
set_property(TARGET eer PROPERTY C_STANDARD 99)

if(PROFILING)
  message("Profiling enabled")
  add_library(profiler STATIC profiler/profiler.c profiler/hash.c
                               profiler/log.c profiler/test_utils.c)
  target_include_directories(profiler PUBLIC profiler include)
  target_link_libraries(eer profiler)
  target_compile_definitions(profiler PUBLIC PROFILING)
elseif(ENABLE_TESTS)
  add_library(logging STATIC profiler/log.c profiler/test_utils.c)
  target_include_directories(logging PUBLIC profiler)
  target_compile_definitions(logging PUBLIC LOGGING)
  target_link_libraries(eer logging)
endif()

if(ENABLE_TESTS)
  # Add tests
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/preprocessor.cmake)
  enable_testing()

  # List of test source files set(TEST_SOURCES test/MyComponent.c
  # test/SimpleTest.c test/LoopHookTest.c)
  aux_source_directory(test TEST_SOURCES)

  # Apply the preprocessor to each test source file
  apply_preprocessor_to_sources(SOURCES ${TEST_SOURCES} TARGET eer)
  add_custom_target(TestSources ALL DEPENDS ${GENERATED_FILES})
  # Create executables from preprocessed files
  foreach(SOURCE ${TEST_SOURCES})
    get_filename_component(SOURCE_NAME ${SOURCE} NAME_WE)
    add_executable(${SOURCE_NAME} ${CMAKE_BINARY_DIR}/${SOURCE_NAME}.e.c)
    add_dependencies(${SOURCE_NAME} TestSources)
    target_include_directories(${SOURCE_NAME} PUBLIC include test)
    target_link_libraries(${SOURCE_NAME} eer)
    add_test(NAME ${SOURCE_NAME} COMMAND ${SOURCE_NAME})
  endforeach()
endif()

# Build examples
if(BUILD_EXAMPLES)
  include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/preprocessor.cmake)

  # List of example source files
  aux_source_directory(examples EXAMPLE_SOURCES)

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
