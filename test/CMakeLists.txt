add_executable (MyComponent MyComponent.c)

# Find Criterion
find_package(Criterion REQUIRED)

target_link_libraries(MyComponent PRIVATE eer Criterion::Criterion)

# use C99
set_property (TARGET MyComponent PROPERTY C_STANDARD 99)

# Enable a few warnings
if (CMAKE_COMPILER_IS_GNUCC)
  target_compile_options (MyComponent PRIVATE -Wall -Wextra)
endif (CMAKE_COMPILER_IS_GNUCC)

add_test (MyComponent MyComponent)
