project(${CMAKE_SYSTEM_PROCESSOR}_${HAL} VERSION 1.0
    DESCRIPTION "eer posix debugging hardware architecture level"
    LANGUAGES C)

include(../hal.cmake)

target_link_libraries(${PROJECT_NAME} hash)
