# EER Framework - Embedded Event-driven Reactor

[![Version](https://img.shields.io/badge/version-0.2.1-blue.svg)](https://github.com/yourusername/eer-framework/releases)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)

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
- **Staging Process**: Manages component transitions through lifecycle states

See the [API Reference](docs/API.md) for detailed information on component definition and the staging process.

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

There are two ways to create an event loop in the EER framework:

**Option 1: Using `loop`**
- `loop(...)` - Start an infinite for-loop with component initialization (checks for `unmounted` flag)
- This is the recommended approach for most applications

**Option 2: Using `ignite`/`terminate`/`halt`**
- `ignite(...)` - Starts a goto-based infinite loop with component initialization
- `terminate` - Exit the current iteration of the event loop
- `halt(code)` - Exit the event loop completely with a return code

**Important:** Choose one approach for your application. Do not mix `loop` with `ignite`/`terminate`/`halt` in the same codebase.

#### Component Registration

There are multiple ways to register components with the event loop, each with a specific purpose:

```c
// Method 1: Pass "always active" components directly to loop
// These components are part of the core loop and don't need prop updates every iteration
loop(backgroundComponent) {
  // Loop body
}

// Method 2: Use apply() for components that need explicit prop updates
loop() {
  apply(UpdatedComponent, updatedComponent, _({.value = newValue}));
}

// Method 3: Use use() for components that are conditionally active
loop() {
  if (condition) {
    use(conditionalComponent);
  }
}
```

**Recommended Pattern:**

For clearer code and better separation of concerns, follow these guidelines:

1. **Pass to `loop()`**: Components that are always active and don't need prop updates every iteration
2. **Use `apply()`**: For components that need explicit prop updates
3. **Use `use()`**: For components that are conditionally active

**How It Works Internally:**

When components are passed to `loop(...)` or `ignite(...)`, the framework:
1. Creates a context for the components
2. Calls `eer_staging` on each component with the `EER_CONTEXT_UPDATED` flag
3. Transitions each component through its lifecycle states (DEFINED → RELEASED → etc.)

When using `use(...)` inside a loop, the same process occurs, just at a different point in the execution flow. The `use` macro calls `eer_staging` on each component with the current context.

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

## How to Use in Your Application

### Getting Started with the Boilerplate

The EER framework includes a boilerplate project structure that you can use as a starting point for your own applications.

1. **Copy the Boilerplate Directory**

   ```bash
   cp -r boilerplate/ my-eer-project/
   cd my-eer-project/
   ```

2. **Build and Run the Example**

   ```bash
   mkdir -p build && cd build
   cmake ..
   make
   ./my_eer_project
   ```

3. **Customize for Your Application**

   - Modify `src/main.c` to define your application's entry point
   - Add new components in `include/components/` and `src/components/`
   - Write tests for your components in `test/`

### Project Structure Best Practices

For larger applications, consider organizing your code as follows:

```
my-app/
├── include/
│   ├── components/     # Component definitions
│   │   ├── ui/         # UI components
│   │   ├── logic/      # Business logic components
│   │   └── system/     # System-level components
│   └── utils/          # Utility functions
├── src/
│   ├── components/     # Component implementations
│   │   ├── ui/
│   │   ├── logic/
│   │   └── system/
│   ├── utils/          # Utility implementations
│   └── main.c          # Application entry point
└── test/               # Test files
```

### Component Design Patterns

1. **Simple Component**

   ```c
   // Header file (include/components/counter.h)
   typedef struct {
     int initial_value;
     int increment;
   } Counter_props_t;

   typedef struct {
     int value;
     int update_count;
   } Counter_state_t;

   eer_header(Counter);

   // Implementation file (src/components/counter.c)
   WILL_MOUNT(Counter) {
     state->value = props->initial_value;
     state->update_count = 0;
   }

   RELEASE(Counter) {
     state->value += props->increment;
     state->update_count++;
   }
   ```

2. **Component Composition**

   ```c
   // Using components inside other components
   typedef struct {
     Counter_t counter;
     Display_t display;
   } Application_state_t;

   RELEASE(Application) {
     // Update child components
     apply(Counter, self->state.counter, _({
       .increment = props->counter_increment
     }));
     
     apply(Display, self->state.display, _({
       .value = self->state.counter.state.value
     }));
   }
   ```

3. **Conditional Rendering**

   ```c
   loop(alwaysActiveComponent) {
     // Conditionally use components
     if (condition) {
       use(conditionalComponent);
     }
     
     // Update components
     apply(AlwaysActiveComponent, alwaysActiveComponent, _({
       .value = newValue
     }));
   }
   ```

