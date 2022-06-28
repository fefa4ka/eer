# executables in use
##########################################################################
set(CMAKE_CROSSTOOL riscv-none-embed)
find_program(RISCV_CC ${CMAKE_CROSSTOOL}-gcc REQUIRED)
find_program(RISCV_CXX ${CMAKE_CROSSTOOL}-g++ REQUIRED)
find_program(RISCV_OBJCOPY ${CMAKE_CROSSTOOL}-objcopy REQUIRED)
find_program(RISCV_OBJDUMP ${CMAKE_CROSSTOOL}-objdump REQUIRED)
find_program(RISCV_SIZE_TOOL ${CMAKE_CROSSTOOL}-size REQUIRED)
find_program(RISCV_STRIP ${CMAKE_CROSSTOOL}-strip REQUIRED)
find_program(RISCV_NM ${CMAKE_CROSSTOOL}-nm REQUIRED)

##########################################################################
# toolchain starts with defining mandatory variables
##########################################################################
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv)
set(CMAKE_C_COMPILER ${RISCV_CC})
set(CMAKE_CXX_COMPILER ${RISCV_CXX})
set(CMAKE_ASM_COMPILER ${RISCV_CC})
set(CMAKE_NM ${RISCV_NN})
set(RISCV 1)

# Builds compiler options
if(CMAKE_BUILD_TYPE MATCHES Release)
    message(STATUS "Maximum optimization for speed")
    set(CMAKE_C_FLAGS_RELEASE "-Ofast")
    #   set(CMAKE_ASM_FLAGS_RELEASE "-Os -x assembler ${CMAKE_RISCV_FLAGS}")
elseif(CMAKE_BUILD_TYPE MATCHES RelWithDebInfo)
    message(STATUS "Maximum optimization for speed, debug info included")
    set(CMAKE_C_FLAGS_RELWITHDEBINFO "-Ofast -save-temps -g -gdwarf-3 -gstrict-dwarf")
elseif (${CMAKE_BUILD_TYPE} MATCHES MinSizeRel)
    message(STATUS "Maximum optimization for size")
    add_compile_options(-Os)
elseif(CMAKE_BUILD_TYPE MATCHES Debug)
    message(STATUS "Without optimization, debug info included")
    # FIXME: -0g for minimal optimization maybe better
    set(CMAKE_C_FLAGS_DEBUG "-O0 -save-temps -g -gdwarf-3 -gstrict-dwarf")
endif()

set(MCU ch573 CACHE STRING "RISCV family MCU")

# The programmer to use, read avrdude manual for list
set(PROG_TYPE openocd CACHE STRING "RISCV programmer")
set(HAL ${MCU})


set(CMAKE_EXE_LINKER_FLAGS "-nostartfiles -Xlinker --gc-sections -Xlinker --print-memory-usage")

add_compile_options(
    -std=gnu99 # C99 standard
    -march=rv32imac
    -mabi=ilp32
    -mcmodel=medany
    -msmall-data-limit=8
    -mno-save-restore
    )

add_compile_options(
    -fmessage-length=0
    -fsigned-char
    -ffunction-sections
    -fdata-sections
    -fno-common
    -Wunused
    )



