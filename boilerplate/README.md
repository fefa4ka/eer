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
