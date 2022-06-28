set(LINKER_SCRIPT /Users/fefa4ka/Development/ee-react/eer-apps/lego/eer/hal/riscv/${HAL}/${HAL}.ld )
## Rename the output to .elf as we will create multiple files
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}.elf
    LINK_FLAGS "-T ${LINKER_SCRIPT} -Wl,-Map,\"${PROJECT_NAME}.map\" --specs=nano.specs --specs=nosys.specs"
    )

set(CMAKE_ASM_COMPILE_OBJECT "<CMAKE_ASM_COMPILER> <DEFINES> <INCLUDES> <FLAGS> -x assembler -c -o <OBJECT> <SOURCE>")
set_property(SOURCE ${ARCH_PATH}/${HAL}.s APPEND PROPERTY COMPILE_OPTIONS "-x" "assembler")
target_sources(${PROJECT_NAME} PUBLIC ${ARCH_PATH}/${HAL}.s)

add_custom_target(${PROJECT_NAME}.lst
    ALL ${RISCV_OBJDUMP} --all-headers --demangle --disassemble ${PROJECT_NAME}.elf > ${PROJECT_NAME}.lst
    DEPENDS ${PROJECT_NAME})

add_custom_target(${PROJECT_NAME}.mcu_usage
    ALL ${RISCV_SIZE_TOOL} --format=berkeley ${PROJECT_NAME}.elf
    DEPENDS ${PROJECT_NAME})

## Strip binary for upload
add_custom_target(${PROJECT_NAME}.strip
    ALL ${RISCV_STRIP} ${PROJECT_NAME}.elf
    DEPENDS ${PROJECT_NAME}.lst)

## Transform binary into hex file, we ignore the eeprom segments in the step
add_custom_target(${PROJECT_NAME}.hex
    COMMAND
    ${RISCV_OBJCOPY} -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    DEPENDS ${PROJECT_NAME}.strip ${PROJECT_NAME}.lst)
#
## Upload the firmware with avrdude
#add_custom_target(upload avrdude  -c ${PROG_TYPE} -p ${MCU} -U flash:w:${PROJECT_NAME}.hex DEPENDS hex)
#
## Upload the eeprom with avrdude
#add_custom_target(upload_eeprom avrdude -c ${PROG_TYPE} -p ${MCU}  -U eeprom:w:${PROJECT_NAME}.eep DEPENDS eeprom)
#
## Burn fuses
#add_custom_target(fuses avrdude -c ${PROG_TYPE} -p ${MCU}  -U lfuse:w:${L_FUSE}:m -U hfuse:w:${H_FUSE}:m -U efuse:w:${E_FUSE}:m -U lock:w:${LOCK_BIT}:m )
#
## Clean extra files
set_directory_properties(PROPERTIES ADDITIONAL_MAKE_CLEAN_FILES "${PROJECT_NAME}.s;${PROJECT_NAME}.i;${PROJECT_NAME}.hex;${PROJECT_NAME}.eeprom;${PROJECT_NAME}.map;${PROJECT_NAME}.lst")
