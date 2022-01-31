add_library(${PROJECT_NAME} STATIC ${HAL}.c)
target_include_directories(${PROJECT_NAME} PUBLIC ../../include .)
target_include_directories(${PROJECT_NAME} INTERFACE .)

#get_property(${PROJECT_NAME}_LINKED_LIBS TARGET ${PROJECT_NAME} PROPERTY LINK_LIBRARIES)
#message("${PROJECT_NAME}_LINKED_LIBS: ${${PROJECT_NAME}_LINKED_LIBS}")

add_custom_target(${PROJECT_NAME}.size
    ${CMAKE_NM} --print-size --size-sort -t d lib${PROJECT_NAME}.a > ${PROJECT_NAME}.size
    DEPENDS ${PROJECT_NAME})

