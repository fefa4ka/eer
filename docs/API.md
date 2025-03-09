# EER Framework API Reference

## Component Lifecycle

The EER framework follows a component-based architecture with a well-defined lifecycle:

```
        ┌───────┐  will_mount  ┌────────┐   should_update?  ┌────────┐
        │DEFINED├─────────────►│RELEASED├──────────────────►│PREPARED├──┐
        └───────┘  release     └────────┘   will_update     └────────┘  │
                   did_mount        ▲                        release    │
                                    ├───────────────────────────────────┘
                                    ▼                        did_update
                               ┌─────────┐
                               │UNMOUNTED│
                               └─────────┘
```

### Lifecycle Hooks

Each component can implement the following lifecycle hooks:

#### `WILL_MOUNT(Type)`
Called before a component is mounted. Use this to initialize state based on props.

```c
WILL_MOUNT(MyComponent) {
  state->initialized = true;
  state->value = props->value;
}
```

#### `SHOULD_UPDATE(Type)`
Determines if a component should update. Return `true` to update, `false` to skip.

```c
SHOULD_UPDATE(MyComponent) {
  // Only update if the value has changed
  return props->value != next_props->value;
}
```

#### `WILL_UPDATE(Type)`
Called before a component updates. Use this to prepare for the update.

```c
WILL_UPDATE(MyComponent) {
  state->previous_value = state->value;
}
```

#### `RELEASE(Type)`
The main update function. Apply props to state here.

```c
RELEASE(MyComponent) {
  state->value = props->value;
}
```

#### `DID_UPDATE(Type)`
Called after a component has updated. Use this for side effects.

```c
DID_UPDATE(MyComponent) {
  printf("Value updated from %d to %d\n", state->previous_value, state->value);
}
```

#### `DID_MOUNT(Type)`
Called after a component has mounted. Use this for initialization that requires the component to be fully mounted.

```c
DID_MOUNT(MyComponent) {
  printf("Component mounted with value %d\n", state->value);
}
```

#### `DID_UNMOUNT(Type)`
Called when a component is unmounted. Use this for cleanup.

```c
DID_UNMOUNT(MyComponent) {
  printf("Component unmounted\n");
}
```

## Component Definition

### Component Structure

Components in the EER framework consist of three main parts:

1. **Props**: Input parameters that control the component's behavior
2. **State**: Internal data that can change over time
3. **Instance**: The core component structure that manages lifecycle

A component is defined using the following pattern:

```c
// Define the props structure
typedef struct {
  int value;
  // Other input properties
} MyComponent_props_t;

// Define the state structure
typedef struct {
  int current_value;
  bool initialized;
  // Other internal state
} MyComponent_state_t;

// Declare the component header
eer_header(MyComponent);

// Implement lifecycle methods
WILL_MOUNT(MyComponent) {
  // Initialize state
}

// Other lifecycle methods...
```

### Component Creation Macros

#### `eer_header(Type)`
Declares a component type with its props and state structures. This macro generates the necessary function prototypes and type definitions.

```c
eer_header(MyComponent);
```

Internally, this expands to:

```c
typedef struct MyComponent {
    eer_t          instance;  // Core component structure
    MyComponent_props_t props;  // Component properties
    MyComponent_state_t state;  // Component state
} MyComponent_t;

// Function prototypes for lifecycle methods
void MyComponent_will_mount(void *instance, void *next_props);
bool MyComponent_should_update(void *instance, void *next_props);
void MyComponent_will_update(void *instance, void *next_props);
void MyComponent_release(void *instance);
void MyComponent_did_mount(void *instance);
void MyComponent_did_unmount(void *instance);
void MyComponent_did_update(void *instance);
```

#### `eer(Type, name)`
Creates a component instance with default props.

```c
eer(MyComponent, myComponent);
```

#### `eer_withprops(Type, name, props)`
Creates a component with specified props.

```c
eer_withprops(MyComponent, myComponent, _({
  .value = 42
}));
```

#### `eer_withstate(Type, name, state)`
Creates a component with specified state.

```c
eer_withstate(MyComponent, myComponent, _({
  .value = 0,
  .initialized = false
}));
```

#### `eer_define(Type, name, props, state)`
Creates a component with both props and state specified.

```c
eer_define(MyComponent, myComponent, 
  _({ .value = 42 }),  // props
  _({ .initialized = false })  // state
);
```

### Component Staging Process

The EER framework uses a staging process to manage component lifecycle transitions. This is handled by the `eer_staging` function, which is the core of the framework's reactivity system.

#### How Staging Works

1. **Component Creation**: When a component is created, it starts in the `EER_STAGE_DEFINED` state.

2. **Mounting**: When the component is first used (via `use()`, `loop()`, etc.), it transitions to:
   - `will_mount()` is called to initialize the component
   - `release()` is called to apply initial props to state
   - `did_mount()` is called for post-mount operations
   - Component enters `EER_STAGE_RELEASED` state

3. **Updates**: When props change (via `apply()` or `react()`):
   - `should_update()` is called to determine if update is needed
   - If update is needed, component enters `EER_STAGE_PREPARED` state
   - `will_update()` is called to prepare for the update
   - `release()` is called to apply new props to state
   - `did_update()` is called for post-update operations
   - Component returns to `EER_STAGE_RELEASED` state

4. **Unmounting**: When a component is unmounted:
   - Component enters `EER_STAGE_UNMOUNTED` state
   - `did_unmount()` is called for cleanup
   - Component enters `EER_STAGE_BLOCKED` state

The staging process ensures that components transition through their lifecycle in a predictable way, with appropriate hooks called at each stage.

#### Implementation Details

The `eer_staging` function in `src/eer.c` handles these transitions:

```c
enum eer_context eer_staging(eer_t *instance, void *next_props)
{
    // Convert pointer to context value if it's a context flag
    uintptr_t context = (uintptr_t)next_props;

    // Handle different context flags
    if(EER_CONTEXT_SAME == context) {
        if(EER_STAGE_RELEASED >= instance->stage.state.step)
            return EER_CONTEXT_SAME;
    } else if(EER_CONTEXT_UPDATED == context) {
        next_props = 0;
    } else if (EER_CONTEXT_BLOCKED == context) {
        instance->stage.state.step = EER_STAGE_UNMOUNTED;
    }

    // Handle different component stages
    if (EER_STAGE_RELEASED == instance->stage.state.step) {
        // Update process
        if(!instance->should_update(instance, next_props)) {
            return EER_CONTEXT_SAME;
        }
        instance->stage.state.step = EER_STAGE_PREPARED;
        instance->will_update(instance, next_props);
    } else if (EER_STAGE_REACTING == instance->stage.state.step) {
        // React process (forced update)
        instance->stage.state.step = EER_STAGE_PREPARED;
        instance->will_update(instance, next_props);
        instance->stage.state.step = EER_STAGE_RELEASED;
        instance->release(instance);
        instance->did_update(instance);
    } else if (EER_STAGE_PREPARED == instance->stage.state.step) {
        // Complete update process
        instance->stage.state.step = EER_STAGE_RELEASED;
        instance->release(instance);
        instance->did_update(instance);
    } else if (EER_STAGE_DEFINED == instance->stage.state.step) {
        // Mount process
#ifdef PROFILING
        hash_write(&eer_scope, eer_hash_component(instance->name), (void **)instance);
#endif
        instance->will_mount(instance, next_props);
        instance->release(instance);
        instance->did_mount(instance);
        instance->stage.state.step = EER_STAGE_RELEASED;
    } else if (EER_STAGE_UNMOUNTED == instance->stage.state.step) {
        // Unmount process
        instance->stage.state.step = EER_STAGE_BLOCKED;
        instance->did_unmount(instance);
    } else if (EER_STAGE_BLOCKED) {
        return EER_CONTEXT_BLOCKED;
    }

    return EER_CONTEXT_UPDATED;
}
```

## Event Loop

The EER framework provides two different approaches to creating an event loop. Choose the one that best fits your application style.

### Approach 1: Using `loop`

#### `loop(...)`
Start the main event loop using a for-loop construct. This is the recommended approach for most applications.

```c
// Method 1: Register components directly in loop
loop(component1, component2) {
  // Loop body - this code runs on each iteration
  
  // Components are automatically initialized and updated
}

// Method 2: Register components with use() inside loop
loop() {
  use(component1, component2);
  // Components are registered in this iteration
  
  // Rest of loop body
}
```

Both methods achieve the same result. The first is more concise, while the second allows for conditional component registration.

### Approach 2: Using `ignite`/`terminate`/`halt`

#### `ignite(...)`
Initialize the event loop with components using a goto-based approach.

```c
ignite(component1, component2);

// Code here runs on each iteration

if (someCondition) {
  terminate; // Skip to next iteration
}

if (shouldExit) {
  eer_land.state.unmounted = true; // Signal that we want to exit
}

halt(0); // Either restart loop or exit with return code 0
```

**How it works internally:**
1. `ignite(...)` creates a label called `eer_boot` and initializes components
2. Your code runs after the `ignite` call
3. `terminate` is a macro that uses `goto eer_boot` to jump back to the beginning
4. `halt(code)` checks `eer_land.state.unmounted`:
   - If `false`: uses `goto eer_boot` to restart the loop
   - If `true`: returns the provided code, exiting the function

The implementation in `eer.h` looks like this:
```c
#define eer_terminate                                                          \
    if (!eer_land.state.unmounted)                                             \
        goto eer_boot;
#define eer_halt(code)                                                         \
    if (!eer_land.state.unmounted)                                             \
        goto eer_boot;                                                         \
    else {                                                                     \
        return code;                                                           \
    }
```

To exit the loop completely, set `eer_land.state.unmounted = true` before calling `halt()`.

## Component Interaction

### `apply(Type, instance, props)`
Apply new props to a component.

```c
apply(MyComponent, myComponent, _({
  .value = 123
}));
```

### `react(Type, instance, props)`
Force a component to react to new props.

```c
react(MyComponent, myComponent, _({
  .value = 123
}));
```

### `use(...)`
Use components in the current context. This registers components with the event loop during execution.

```c
use(component1, component2);
```

This is equivalent to passing components directly to `loop()` or `ignite()`, but can be called at any point in the execution flow. It's useful for conditional component registration:

```c
loop() {
  // Always use these components
  use(alwaysNeededComponent);
  
  // Conditionally use other components
  if (condition) {
    use(conditionalComponent);
  }
}
```

### Component Registration Best Practices

For clearer code organization and better separation of concerns, follow these guidelines:

1. **Pass to `loop()`**: Components that are always active and don't need prop updates every iteration
   ```c
   // These components are part of the core loop
   loop(backgroundComponent, systemComponent) {
     // Loop body
   }
   ```

2. **Use `apply()`**: For components that need explicit prop updates
   ```c
   loop() {
     // Explicitly update this component's props
     apply(UpdatedComponent, updatedComponent, _({.value = newValue}));
   }
   ```

3. **Use `use()`**: For components that are conditionally active
   ```c
   loop() {
     // Only register this component when needed
     if (condition) {
       use(conditionalComponent);
     }
   }
   ```

This pattern creates a clearer mental model of your application's structure:
- Components in `loop()` are "registered" with the event loop for the entire application lifecycle
- Operations inside the loop body are explicit actions on components
- Conditional component usage is clearly expressed with `use()`

### `with(...)`
Create a new context with specified components, context body updates if components change.

```c
with(component1, component2) {
  // Context body
}
```
