# My EER Project

This is a project using the EER (Embedded Event-driven Reactor) framework.

## Project Structure

```
.
├── CMakeLists.txt       # CMake build configuration
├── Makefile             # Alternative simple Makefile
├── include/             # Header files
│   └── components/      # Component definitions
│       └── my_component.h
├── src/                 # Source files
│   ├── main.c           # Application entry point
│   └── components/      # Component implementations
│       └── my_component.c
└── test/                # Test files
    └── my_component_test.c
```

## Building the Project

### Using CMake

```bash
mkdir -p build && cd build
cmake ..
make
```

### Using Makefile

```bash
make
```

## Running the Application

```bash
./build/app
```

## Running Tests

```bash
make test
```

Or with CMake:

```bash
mkdir -p build && cd build
cmake -DENABLE_TESTS=ON ..
make
ctest
```

## Adding New Components

1. Create a header file in `include/components/`
2. Create an implementation file in `src/components/`
3. Include the component in your application
# EER Boilerplate Project

This is a starter project for the EER (Embedded Event-driven Reactor) framework.

## Getting Started

### Building the Project

```bash
mkdir -p build && cd build
cmake ..
make
```

### Running the Project

```bash
./my_eer_project
```

## Project Structure

```
.
├── CMakeLists.txt           # Build configuration
├── include/                 # Header files
│   └── components/          # Component definitions
│       └── my_component.h   # Example component
├── src/                     # Source files
│   ├── components/          # Component implementations
│   │   └── my_component.c   # Example component implementation
│   └── main.c               # Application entry point
└── test/                    # Test files
```

## Creating Components

To create a new component:

1. Define the component in `include/components/your_component.h`
2. Implement the component in `src/components/your_component.c`
3. Use the component in your application

## Component Lifecycle

The EER framework provides these lifecycle hooks:

- `WILL_MOUNT`: Called before a component is mounted
- `SHOULD_UPDATE`: Determines if a component should update
- `WILL_UPDATE`: Called before a component updates
- `RELEASE`: The main update function
- `DID_UPDATE`: Called after a component has updated
- `DID_MOUNT`: Called after a component has mounted
- `DID_UNMOUNT`: Called when a component is unmounted

## Example

```c
// In your main.c
#include <eer.h>
#include <eer_app.h>
#include "components/my_component.h"

int main() {
    // Create component
    eer_withprops(MyComponent, myComponent, _({.value = 1}));

    // Start event loop
    loop(myComponent) {
        // Update component
        apply(MyComponent, myComponent, _({.value = myComponent.state.value + 1}));
        
        // Exit condition
        if (myComponent.state.update_count >= 5) {
            eer_land.state.unmounted = true;
        }
    }

    return 0;
}
```
