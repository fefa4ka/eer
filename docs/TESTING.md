# EER Framework Testing Guide

## Overview

The EER Framework includes a built-in testing framework designed specifically for testing component-based applications. This document explains how to use the testing framework to validate your components and applications.

## Test Framework Architecture

The testing framework uses a multi-threaded approach to test components:

1. The main program runs in one thread
2. Each test runs in its own separate thread
3. Tests can run simultaneously with the main program
4. When all tests complete, the main program is automatically unmounted

## Key Testing Macros

### `test(...)`

The `test(...)` macro is the primary entry point for defining tests. It can accept multiple test functions that will run simultaneously.

```c
test(test_function1, test_function2, test_function3) {
    // Main program code here
    loop(component1, component2) {
        // Component logic
    }
}
```

In this example:
- `test_function1`, `test_function2`, and `test_function3` will run in parallel
- The main program with the event loop runs in its own thread
- When all test functions complete, the main program will be unmounted

### Test Function Structure

Each test function should:
1. Return a `result_t` value (`OK` or an error code)
2. Use `test_assert()` to validate conditions
3. Be declared before the `test(...)` macro

```c
result_t test_function1() {
    test_assert(component.state.value == 42, "Component value should be 42");
    return OK;
}
```

### `test_assert()`

The `test_assert()` macro validates conditions and logs the results:

```c
test_assert(condition, message, ...);
```

- If the condition is true, it logs an "OK" message
- If the condition is false, it logs an "ERROR" message and returns `ERROR_UNKNOWN`

## How It Works Internally

The test framework uses several key mechanisms:

### Thread Management

```c
// Define test functions
test_define(test_function1, test_function2);

// Create threads for each test
test_execute(test_function1, test_function2);

// Wait for all test threads to complete
test_wait(test_function1, test_function2);
```

### Automatic Unmounting

When all tests complete, the framework sets `eer_land.state.unmounted = true`, which signals the event loop to exit:

```c
// After all tests complete
eer_land.state.unmounted = true;
pthread_join(program_thread, NULL);
```

### Logging

The framework includes a logging system that records:
- Test assertions (pass/fail)
- Timestamps
- File and line information for errors

## Example Test

```c
#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
#include "test.h"

// Define component
typedef struct {
  int value;
} TestComponent_props_t;

typedef struct {
  int value;
  bool initialized;
} TestComponent_state_t;

eer_header(TestComponent);

// Implement lifecycle methods
WILL_MOUNT(TestComponent) {
  state->initialized = true;
  state->value = props->value;
}

// Create component instance
eer_withprops(TestComponent, testComponent, _({.value = 42}));

// Define test function
result_t verify_component() {
  test_assert(testComponent.state.initialized == true, 
              "Component should be initialized");
  test_assert(testComponent.state.value == 42, 
              "Component value should be 42");
  return OK;
}

// Main test program
test(verify_component) {
  loop(testComponent) {
    // The loop will automatically mount the component
    // Tests will run in parallel
  }
}
```

## Best Practices

1. **Test Isolation**: Each test should be independent and not rely on the state of other tests
2. **Timing Considerations**: Use `usleep()` when necessary to allow the main program to process events
3. **Assertion Messages**: Provide clear, descriptive messages in assertions
4. **Test Coverage**: Test all lifecycle methods of your components
5. **Test Both Event Loop Approaches**: Test both `loop` and `ignite` approaches

## Debugging Tests

When a test fails:
1. Check the log output for the specific assertion that failed
2. Look at the file and line number in the error message
3. Use `log_info()` to add additional debug information to your tests

## Running Tests

Tests are built and run using CMake:

```bash
mkdir -p build && cd build
cmake -DENABLE_TESTS=ON -DCMAKE_BUILD_TYPE=Debug ..
make
ctest
```

Or run individual tests:

```bash
./TestName
```

## Profiling

The EER Framework includes built-in profiling capabilities to help identify performance bottlenecks and optimize your components.

### Enabling Profiling

To enable profiling, build your project with the `PROFILING` option:

```bash
mkdir -p build && cd build
cmake -DPROFILING=ON -DCMAKE_BUILD_TYPE=Debug ..
make
```

### Profiling Output

When profiling is enabled:

1. Each component's lifecycle methods are timed
2. CPU usage is tracked for each method
3. Call counts are recorded
4. Hardware abstraction layer (HAL) calls are monitored

The profiling data is automatically dumped when:
- The program exits normally
- You press Ctrl+C (SIGINT is caught)
- The `eer_dump_usage()` function is called explicitly

### Profiling Report

The profiling report includes:

```
CPU usage details:

operation               steps   cpu     %

3 components            1000    12345   100%

0: MyComponent / Button
 will_mount             5       123     0.99%
   some_call_counter    10
 should_update          100     456     3.69%
 will_update            95      789     6.39%
 release                95      1011    8.19%
 did_update             95      234     1.90%
                                2613    21.17%


1: AnotherComponent / Slider
...
```

This shows:
- Total number of components and event loop steps
- Per-component CPU usage breakdown
- Per-lifecycle-method timing and percentage of total CPU time
- Hardware call counts within each method

### Analyzing Profiling Data

Use the profiling data to:

1. **Identify Bottlenecks**: Look for methods consuming a high percentage of CPU time
2. **Optimize Update Logic**: Check if `should_update` is properly filtering unnecessary updates
3. **Reduce Hardware Calls**: Look for excessive hardware calls in frequently executed methods
4. **Balance Component Load**: Ensure no single component dominates CPU usage

### Custom Profiling Points

You can add custom profiling points in your code:

```c
#ifdef PROFILING
  // Custom profiling code
  eer_profiler_counter(some_custom_counter);
#endif
```

### Memory Profiling

The profiling system also tracks component allocation. Check the report for:
- Number of components created
- Component lifecycle patterns
- Potential memory leaks (components that never unmount)
