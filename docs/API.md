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

### `eer_header(Type)`
Declares a component type with its props and state structures.

```c
eer_header(MyComponent);
```

### `eer(Type, name)`
Creates a component instance with default props.

```c
eer(MyComponent, myComponent);
```

### `eer_withprops(Type, name, props)`
Creates a component with specified props.

```c
eer_withprops(MyComponent, myComponent, _({
  .value = 42
}));
```

### `eer_withstate(Type, name, state)`
Creates a component with specified state.

```c
eer_withstate(MyComponent, myComponent, _({
  .value = 0,
  .initialized = false
}));
```

## Event Loop

The EER framework provides two different approaches to creating an event loop. Choose the one that best fits your application style.

### Approach 1: Using `loop`

#### `loop(...)`
Start the main event loop using a for-loop construct. This is the recommended approach for most applications.

```c
loop(component1, component2) {
  // Loop body - this code runs on each iteration
  
  // Components are automatically initialized and updated
}
```

### Approach 2: Using `ignite`/`terminate`/`halt`

#### `ignite(...)`
Initialize the event loop with components using a goto-based approach.

```c
ignite(component1, component2);

// Code here runs on each iteration

halt(0); // Restart loop or exit program if `eer_land.state.unmounted == true` 
```

`halt()` restarts the loop so should be used after main code has finished.

Use `eer_land.state.unmounted` to force the loop to exit.

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
Use components in the current context.

```c
use(component1, component2);
```

### `with(...)`
Create a new context with specified components.

```c
with(component1, component2) {
  // Context body
}
```
