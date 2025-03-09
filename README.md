# EER Framework - Embedded Event-driven Reactor

A lightweight framework for building reactive, component-based embedded systems. Provides a structured approach to managing hardware interactions and system reactivity with minimal overhead.

## Key Features

- 🧩 Component-based architecture with finite state machines
- ⚡ Event-driven lifecycle management 
- 🔄 Automatic reactivity system with dependency tracking
- 📦 Memory-efficient design (<2KB RAM typical usage)
- ⏱️ Deterministic execution with cycle-counted operations

## Getting Started

This example shows how to create a simple application using the EER framework:

```c
#include <eer.h>
#include <eers_app.h>

#include <MyComponent.h>

/* Initialize the component with some initial props */
MyComponent(myComponentInstance, _({
  .value = 42  // Initial value for the component
}));

void main() {
    // Start the event loop - this is where the magic happens
    loop() { 
        /* Apply new properties to trigger component updates
           The component will automatically re-render when props change */
        apply(MyComponent, myComponentInstance, _({
          .value = 123  // New value that will trigger an update
        }));
        
        // The framework handles the component lifecycle automatically
    }
}
```

The EER framework manages the component lifecycle for you:
1. Components are initialized with initial props
2. The event loop processes updates and triggers lifecycle methods
3. Components react to property changes automatically
4. Memory usage is optimized for embedded systems

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

#### Component Definition
- `eer_header(Type)` - Declares a component type with its props and state structures
- `eer(Type, name)` - Creates a component instance with default props
- `eer_withprops(Type, name, props)` - Creates a component with specified props
- `eer_withstate(Type, name, state)` - Creates a component with specified state

#### Lifecycle Hooks
- `WILL_MOUNT(Type)` - Called before a component is mounted
- `WILL_UPDATE(Type)` - Called before a component updates
- `SHOULD_UPDATE(Type)` - Determines if a component should update (returns boolean)
- `RELEASE(Type)` - Called when a component is being released (main update logic)
- `DID_UPDATE(Type)` - Called after a component has updated
- `DID_MOUNT(Type)` - Called after a component has mounted
- `DID_UNMOUNT(Type)` - Called when a component is unmounted

#### Event Loop
- `loop(...)` - Start an infinite for-loop with component initialization (checks for `unmounted` flag)

Alternatively, you can use the following functions:
- `ignite(...)` - Alternative approach: starts a goto-based infinite loop with component initialization
- `terminate` - Exit the event loop
- `halt(code)` - Exit the event loop with a return code

**Note:** You should use either `loop` or the `ignite`/`halt` pair, not both together.

#### Component Interaction
- `apply(Type, instance, props)` - Apply new props to a component
- `react(Type, instance, props)` - Force a component to react to new props
- `use(...)` - Use components in the current context
- `with(...)` - Create a new context with specified components

### Building and Testing

#### Build Configuration
```bash
# Debug build with tests and profiling enabled
mkdir -p build && cd build
cmake -DENABLE_TESTS=ON -DPROFILING=ON -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=1 ..

# Release build
mkdir -p build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

#### Running Tests
```bash
# Build the tests
cd build
make

# Run all tests
ctest

# Run a specific test
./MyComponent
```
