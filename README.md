# EER Framework - Embedded Event-driven Reactor

A lightweight framework for building reactive, component-based embedded systems. Provides a structured approach to managing hardware interactions and system reactivity with minimal overhead.

## Key Features

- 🧩 Component-based architecture with finite state machines
- ⚡ Event-driven lifecycle management 
- 🔄 Automatic reactivity system with dependency tracking
- 📦 Memory-efficient design (<2KB RAM typical usage)
- ⏱️ Deterministic execution with cycle-counted operations

## Getting Started

```c
#include <eer.h>
#include <eers_app.h>

#include <MyComponent.h>

/* Initialize the component with some initial props */
MyComponent(myComponentInstance, _({
  .value = 42
}));

void main() {
    loop() { 
        /* Apply new properties */
        apply(MyComponent, myComponentInstance, _({
          .value = 123
        }));
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
```


## Documentation

### Core Concepts
- **Components**: Encapsulated units of functionality with props/state
- **Lifecycle Hooks**: will_mount(), did_update(), etc for state management
- **Reactivity System**: Automatic dependency tracking and propagation

### API Reference
- Marcos for event-loop usage (`loop`, `ignite` & `halt`)
- Macros for declaring components (`eer_header`, ...)
- Component lifecycle macros (`WILL_MOUNT`, `DID_UPDATE`, etc)

### Build Tests
```bash
cmake -DENABLE_TESTS=On -DPROFILING=On -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ../
```
