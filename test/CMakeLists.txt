add_executable (MyComponent MyComponent.c)

target_link_libraries(MyComponent PRIVATE eer)

# use C99
set_property (TARGET MyComponent PROPERTY C_STANDARD 99)

add_test (MyComponent MyComponent)
