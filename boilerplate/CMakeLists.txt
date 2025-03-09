cmake_minimum_required(VERSION 3.12)
project(my_eer_project C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Path to EER framework (adjust as needed)
set(EER_FRAMEWORK_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../")

# Include EER framework headers
include_directories(
    ${EER_FRAMEWORK_PATH}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Add source files
file(GLOB_RECURSE APP_SOURCES "src/*.c")

# Create executable
add_executable(${PROJECT_NAME} ${APP_SOURCES})

# Link with EER framework
add_library(eer STATIC IMPORTED)
set_target_properties(eer PROPERTIES
    IMPORTED_LOCATION "${EER_FRAMEWORK_PATH}/build/libeer.a"
)
target_link_libraries(${PROJECT_NAME} eer)

# Optional: Enable testing
option(ENABLE_TESTS "Enable tests" OFF)
if(ENABLE_TESTS)
    enable_testing()
    file(GLOB_RECURSE TEST_SOURCES "test/*.c")
    foreach(TEST_SOURCE ${TEST_SOURCES})
        get_filename_component(TEST_NAME ${TEST_SOURCE} NAME_WE)
        add_executable(${TEST_NAME} ${TEST_SOURCE})
        target_link_libraries(${TEST_NAME} eer)
        add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    endforeach()
endif()
