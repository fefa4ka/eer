project(${CMAKE_SYSTEM_PROCESSOR}_${HAL} VERSION 1.0
    DESCRIPTION "eer ${CMAKE_SYSTEM_PROCESSOR}${HAL} HAL"
    LANGUAGES C)


function(add_hal_library system)
    set(HAL_SYSTEM ${CMAKE_SYSTEM_PROCESSOR}_${HAL}_${system})
    add_library(${HAL_SYSTEM} STATIC ${system}.c log.c)
    target_include_directories(${HAL_SYSTEM} PUBLIC ${EER_LIB_PATH}/include . ${CMAKE_CURRENT_BINARY_DIR})
    target_include_directories(${HAL_SYSTEM} INTERFACE .)
endfunction()


add_hal_library(gpio)
target_link_libraries(${CMAKE_SYSTEM_PROCESSOR}_${HAL}_gpio hash)
target_sources(${CMAKE_SYSTEM_PROCESSOR}_${HAL}_gpio PRIVATE log.c vcd.c)

add_hal_library(uart)
add_hal_library(timer)
add_hal_library(sys)
add_hal_library(ble)
