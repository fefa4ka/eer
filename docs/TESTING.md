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

The `test(...)` macro is the primary entry point for defining tests. It can accept multiple test functions that will run simultaneously. Only single `test` function is allowed.

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

## Loop Hooks and Event Testing

The EER Framework provides a mechanism to hook into specific loop events, making it easier to test component behavior at different stages of execution.

### Available Hook Points

- **Before Loop Start**: Execute code before the event loop begins
- **After Specific Iteration**: Execute code after a specific iteration completes
- **Before Loop Exit**: Execute code just before the loop exits

### Using Loop Hooks

```c
// Define hook functions
void before_loop_hook(void* data) {
  // Code to run before the loop starts
  int* value_ptr = (int*)data;
  *value_ptr = component.state.value;
}

void after_iteration_hook(void* data) {
  // Code to run after a specific iteration
  int* value_ptr = (int*)data;
  *value_ptr = component.state.value;
}

// Register hooks
int before_value = 0;
int after_value = 0;

test_hook_before_loop(before_loop_hook, &before_value);
test_hook_after_iteration(3, after_iteration_hook, &after_value);

// In your test function
test(verify_hooks) {
  loop(component) {
    // Your loop code here
    
    // Exit after some iterations
    if (eer_current_iteration >= 5) {
      break;
    }
  }
}

// Verify hook results
result_t verify_hooks() {
  // Wait for iterations to complete if needed
  test_wait_for_iteration(5);
  
  test_assert(before_value == expected, "Before hook value check");
  test_assert(after_value == expected, "After iteration hook value check");
  
  return OK;
}
```

### Waiting for Iterations

You can wait for a specific iteration to complete before continuing with your test:

```c
// Wait for the 3rd iteration to complete
test_wait_for_iteration(3);

// Now verify the component state after 3 iterations
test_assert(component.state.value == expected, "Value after 3 iterations");
```

### Available Hook Macros

- `test_hook_before_loop(function, data)`: Register a hook before the loop starts
- `test_hook_after_iteration(n, function, data)`: Register a hook after iteration n
- `test_hook_before_exit(function, data)`: Register a hook before the loop exits
- `test_wait_for_iteration(n)`: Wait for iteration n to complete

### Iteration Counter

The current iteration is available via the `eer_current_iteration` variable, which can be used in your loop code:

```c
loop(component) {
  // Do something based on the current iteration
  if (eer_current_iteration == 2) {
    // Special handling for iteration 2
  }
}
```

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

