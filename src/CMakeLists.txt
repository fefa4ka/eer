set (SOURCES
  eer.c
)

add_library (eer ${SOURCES})

# use C99
set_property(TARGET eer PROPERTY C_STANDARD 99)

# Enable a few warnings
if (CMAKE_COMPILER_IS_GNUCC)
  target_compile_options(eer PRIVATE -Wall -Wextra)
endif (CMAKE_COMPILER_IS_GNUCC)
