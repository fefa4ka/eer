# Preprocessing
function(add_c_preprocessor_command)
    # Add custom command to run C preprocessor.
    #
    # Arguments
    #   OUTPUT          output file
    #   SOURCE          input file
    #   TARGET          CMake target to inherit compile definitions, include directories, and compile options
    #   EXTRA_C_FLAGS   extra compiler flags added after all flags inherited from the TARGET

    set(one_value_args TARGET SOURCE OUTPUT)
    set(multi_value_args EXTRA_C_FLAGS)
    cmake_parse_arguments(CPP "" "${one_value_args}" "${multi_value_args}" ${ARGN})

    string(TOUPPER ${CMAKE_BUILD_TYPE} build_type)
    string(REPLACE " " ";" c_flags "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_${build_type}}")

    add_custom_command(
        TARGET ${CPP_TARGET}
        COMMENT "Preprocessed file ${CPP_SOURCE}"
        PRE_BUILD
        COMMAND ${CMAKE_C_COMPILER}
            "-D$<JOIN:$<TARGET_PROPERTY:${CPP_TARGET},COMPILE_DEFINITIONS>,;-D>"
            "-I$<JOIN:$<TARGET_PROPERTY:${CPP_TARGET},INCLUDE_DIRECTORIES>,;-I>"
            ${c_flags} -isystem /usr/include -isystem /usr/local/include
            $<TARGET_PROPERTY:${CPP_TARGET},COMPILE_OPTIONS>
            ${CPP_EXTRA_C_FLAGS}
            -E ${CMAKE_CURRENT_SOURCE_DIR}/${CPP_SOURCE} -o ${CPP_OUTPUT}
        COMMAND_EXPAND_LISTS VERBATIM
        IMPLICIT_DEPENDS C ${CPP_SOURCE}
        DEPENDS ${CPP_SOURCE})
endfunction()

function(apply_preprocessor_to_sources)
    # Apply the preprocessor to a list of source files.
    #
    # Arguments
    #   SOURCES        list of source files
    #   TARGET         CMake target to inherit compile definitions, include directories, and compile options

    set(one_value_args TARGET)
    set(multi_value_args SOURCES)
    cmake_parse_arguments(PP "" "${one_value_args}" "${multi_value_args}" ${ARGN})

    foreach(SOURCE ${PP_SOURCES})
        get_filename_component(SOURCE_NAME ${SOURCE} NAME_WE)
        add_c_preprocessor_command(
            OUTPUT ${SOURCE_NAME}.i
            SOURCE ${SOURCE}
            TARGET ${PP_TARGET}
        )
        add_custom_command(
            TARGET ${PP_TARGET}
            PRE_BUILD
            COMMAND grep "^[^\\#].*\$ " ${SOURCE_NAME}.i > ${SOURCE_NAME}.eu.c
            COMMENT "Cleaning preprocessed file"
            DEPENDS ${SOURCE_NAME}.i
        )
        add_custom_command(
            TARGET ${PP_TARGET}
            PRE_BUILD
            COMMAND clang-format ${SOURCE_NAME}.eu.c > ${SOURCE_NAME}.e.c
            COMMENT "Formatting preprocessed file"
            DEPENDS ${SOURCE_NAME}.eu.c
        )
    endforeach()
endfunction()

