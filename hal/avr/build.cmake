## Rename the output to .elf as we will create multiple files
set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    OUTPUT_NAME ${PROJECT_NAME}.elf
    LINK_FLAGS "-Wl,--gc-sections -mrelax -Wl,-Map,${PROJECT_NAME}.map"
    )

add_custom_target(${PROJECT_NAME}.lst
    ALL ${AVR_OBJDUMP} -h -S ${PROJECT_NAME}.elf > ${PROJECT_NAME}.lst
    DEPENDS ${PROJECT_NAME})

add_custom_target(${PROJECT_NAME}.mcu_usage
    ALL ${AVR_SIZE_TOOL} --format=avr --mcu=${CMAKE_SYSTEM_NAME}${CMAKE_SYSTEM_PROCESSOR} ${PROJECT_NAME}.elf
    DEPENDS ${PROJECT_NAME})

## Strip binary for upload
add_custom_target(${PROJECT_NAME}.strip
    ALL ${AVR_STRIP} ${PROJECT_NAME}.elf
    DEPENDS ${PROJECT_NAME}.lst)

## Transform binary into hex file, this is the eeprom part (empty if you don't
## use eeprom static variables)
add_custom_target(${PROJECT_NAME}.eeprom
    ${AVR_OBJCOPY} -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 --no-change-warnings -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.eeprom
    DEPENDS ${PROJECT_NAME}.strip)

## Transform binary into hex file, we ignore the eeprom segments in the step
add_custom_target(${PROJECT_NAME}.hex
    COMMAND
    ${AVR_OBJCOPY} -R .eeprom -O ihex ${PROJECT_NAME}.elf ${PROJECT_NAME}.hex
    DEPENDS ${PROJECT_NAME}.strip ${PROJECT_NAME}.eeprom ${PROJECT_NAME}.lst)
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
