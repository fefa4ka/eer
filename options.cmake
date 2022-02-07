execute_process(COMMAND date "+%s" OUTPUT_VARIABLE BUILD_TIMESTAMP)

if(NOT CMAKE_CROSSCOMPILING)
    set(HAL "dbg")
endif()

if(NOT CMAKE_CROSSCOMPILING AND CMAKE_BUILD_TYPE MATCHES Debug)
    option(PROFILING "Profiling" ON)
endif()

set(EER_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}${EER_PATH}/")
set(EER_HAL_PATH "hal/${CMAKE_SYSTEM_PROCESSOR}")
if(EXISTS "${EER_LIB_PATH}/${EER_HAL_PATH}/${HAL}/CMakeLists.txt")
    set(EER_HAL_PATH "hal/${CMAKE_SYSTEM_PROCESSOR}/${HAL}")
endif()
set(EER_HAL_LIB "${CMAKE_SYSTEM_PROCESSOR}_${HAL}")
set(ARCH_PATH "${EER_LIB_PATH}/${EER_HAL_PATH}")

configure_file(${EER_LIB_PATH}/include/version.h.in version.h)
