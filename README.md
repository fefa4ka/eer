# EER Framework - Embedded Event-driven Reactor

A lightweight framework for building reactive, component-based embedded systems. Provides a structured approach to managing hardware interactions and system reactivity with minimal overhead.

## Key Features

- 🧩 Component-based architecture with finite state machines
- ⚡ Event-driven lifecycle management with ISR-safe operations
- 🔄 Automatic reactivity system with dependency tracking
- 🛠️ Hardware abstraction layer for portable peripherals
- 📦 Memory-efficient design (<2KB RAM typical usage)
- ⏱️ Deterministic execution with cycle-counted operations
- 🔒 Thread-safe primitives for RTOS integration

## Getting Started

### Prerequisites
- ARM GCC toolchain (9-2020-q2-update recommended)
- CMake 3.12+ for build configuration
- Segger J-Link tools for flashing/debugging

```c
#include <eer.h>
#include <eers_app.h>
#include "hal/gpio.h"

// Define a LED component
EER_COMP(Blinker, ({
    .gpio = hw_pin(A, 0),
    .interval = 1000
}));

void main() {
    ignite(Blinker); // Initialize components
    
    loop(Blinker) { // Main event loop
        // Toggle LED every interval
        apply(Blinker, _({
            .gpio.level = !eer_state(Blinker, Blinker)->level
        }));
        
        // Schedule next toggle
        eer_hw_systick_delay(Blinker.props.interval);
    }
}
```

### Component Declaration
```c
#include <eer_comp.h>

/* Declare the component type */
eer_header(MyComponent);

/* Define the component's state and properties */
typedef struct {
  int value;
} MyComponent_props_t;

typedef struct {
  int value;
  bool initialized;
  bool updated;
} MyComponent_state_t;


/* Implement the component's lifecycle methods */

WILL_MOUNT(MyComponent) {
  /* Initialize the component's state */
  state->initialized = true;
}
WILL_UPDATE(MyComponent) {
  /* Update the component's state */
  state->updated = false;
}

RELEASE(MyComponent) {
  /* Update the component's state based on the new props */
  state->value = props->value;
}

DID_UPDATE(MyComponent) {
  /* Perform any tasks that need to be done after the component is released */
  state->updated = true;
  printf("MyComponent updated with value %d\n", state->value);
}

DID_MOUNT(MyComponent) {
  /* Perform any tasks that need to be done after the component is fully initialized */
  printf("MyComponent mounted with value %d\n", state->value);
}

/* Implement any additional functions or methods for the component */

void MyComponent_reset(eer_t *instance) {
  /* Reset the component's state */
  MyComponent_state_t *state = eer_state(MyComponent, instance);
  state->value = 0;
}

int MyComponent_getValue(eer_t *instance) {
  /* Get the current value of the component */
  MyComponent_state_t *state = eer_state(MyComponent, instance);
  return state->value;
}
```


## Documentation

### Core Concepts
- **Components**: Encapsulated units of functionality with props/state
- **Lifecycle Hooks**: will_mount(), did_update(), etc for state management
- **Reactivity System**: Automatic dependency tracking and propagation
- **Hardware Abstraction**: Uniform interface for peripherals (GPIO, I2C, etc)

### API Reference
- Component lifecycle macros (`WILL_MOUNT`, `DID_UPDATE`, etc)
- Hardware abstraction layer (hal/gpio.h, hal/i2c.h)
- Memory management utilities (pool allocator, circular buffers)

### Simulation Example
```bash
# Build and run desktop simulation
cmake -B build -DPLATFORM=simulation
cmake --build build && ./build/simulator
```

## License
