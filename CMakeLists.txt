cmake_minimum_required(VERSION 3.1)

project(eeReact VERSION 1.0
    DESCRIPTION "Embedded Event-driven React"
    LANGUAGES C)

set(EER_PATH "")
include(options.cmake)

message(STATUS "Platform: ${CMAKE_SYSTEM_NAME}/${CMAKE_SYSTEM_PROCESSOR}/${HAL}")

add_library(hash STATIC src/hash.c)
target_include_directories(hash PUBLIC include)

add_library(lr STATIC src/lr.c)
target_include_directories(lr PUBLIC include ${EER_HAL_PATH} ${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(${EER_HAL_PATH})

add_library(eer STATIC src/eer.c)
target_include_directories(eer PUBLIC include ${EER_HAL_PATH} ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(eer INTERFACE include)
target_link_libraries(eer)

if(${PROFILING})
    message("Profiling enabled")
    add_library(profiling STATIC src/profiler.c)
    target_include_directories(profiling PUBLIC include ${EER_HAL_PATH} ${CMAKE_CURRENT_BINARY_DIR})
    target_link_libraries(eer profiling)
endif()

