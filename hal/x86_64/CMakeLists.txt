project(${CMAKE_SYSTEM_PROCESSOR}_${HAL} VERSION 1.0
    DESCRIPTION "eer posix debugging hardware architecture level"
    LANGUAGES C)

if(EXISTS "${PROJECT_SOURCE_DIR}/${HAL}/CMakeLists.txt")
    add_subdirectory(${HAL})
else()
    add_library(${PROJECT_NAME} STATIC ${HAL}.c)
    target_include_directories(${PROJECT_NAME} PUBLIC ../../../include .)
    target_include_directories(${PROJECT_NAME} INTERFACE .)
    target_link_libraries(${PROJECT_NAME} hash)
endif()
