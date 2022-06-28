project(${CMAKE_SYSTEM_PROCESSOR}_${HAL} VERSION 1.0
    DESCRIPTION "eer ${CMAKE_SYSTEM_PROCESSOR}${HAL} HAL"
    LANGUAGES C)

function(add_hal_library system)
    set(HAL_SYSTEM ${CMAKE_SYSTEM_PROCESSOR}_${HAL}_${system})
    add_library(${HAL_SYSTEM} STATIC ${system}.c)
    target_include_directories(${HAL_SYSTEM} PUBLIC ../../include .)
    target_include_directories(${HAL_SYSTEM} INTERFACE .)
endfunction()

add_hal_library(gpio)
add_hal_library(adc)
add_hal_library(uart)
add_hal_library(spi)
add_hal_library(timer)
