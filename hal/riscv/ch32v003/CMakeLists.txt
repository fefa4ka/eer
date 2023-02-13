project(${CMAKE_SYSTEM_PROCESSOR}_${HAL} VERSION 1.0
    DESCRIPTION "eer ${CMAKE_SYSTEM_PROCESSOR}${HAL} HAL"
    LANGUAGES C)

find_program(OPENOCD openocd REQUIRED)
set(OPENOCD_CFG "${EER_LIB_PATH}/hal/riscv/${HAL}/wch-riscv.cfg")

function(add_hal_library system)
    set(HAL_SYSTEM ${CMAKE_SYSTEM_PROCESSOR}_${HAL}_${system})
    add_library(${HAL_SYSTEM} STATIC ${system}.c ${HAL}.c)
    target_include_directories(${HAL_SYSTEM} PUBLIC ../../../include .)
    target_include_directories(${HAL_SYSTEM} INTERFACE .)
endfunction()

add_hal_library(gpio)
add_hal_library(sys)
add_hal_library(timer)
add_hal_library(uart)
target_link_libraries(${CMAKE_SYSTEM_PROCESSOR}_${HAL}_uart PUBLIC ${CMAKE_SYSTEM_PROCESSOR}_${HAL}_gpio)

add_custom_target(ch32v003.erase
    COMMAND
    ${OPENOCD} -f ${OPENOCD_CFG} -c init -c halt -c \"flash erase_sector wch_riscv 0 last\" -c exit
)

add_custom_target(ch32v003.reset
    COMMAND
    ${OPENOCD} -f ${OPENOCD_CFG} -c init -c reset -c resume -c exit
)

add_custom_target(ch32v003.monitor
    COMMAND
    ${OPENOCD} -f ${OPENOCD_CFG} &
)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/.gdbinit
     DESTINATION ${CMAKE_BINARY_DIR})

