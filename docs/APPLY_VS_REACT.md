# Understanding Apply vs React in EER Framework

## Overview

The EER framework provides two primary methods for updating components:

1. **`apply()`**: Updates a component across two loop iterations
2. **`react()`**: Forces an immediate update in a single loop iteration

This document explains the differences between these approaches and why the framework is designed to split lifecycle execution across iterations.

## The Component Lifecycle

To understand the difference, let's first review the component lifecycle:

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

## Apply: Two-Phase Updates

When you call `apply(Type, component, props)`, the update is split across two loop iterations:

### First Iteration
1. Check if the component should update (`should_update()`)
2. If yes, prepare for the update (`will_update()`)
3. Move to `PREPARED` state

### Second Iteration
1. Apply the changes (`release()`)
2. Complete the update (`did_update()`)
3. Return to `RELEASED` state

### Example:

```c
// First iteration: Prepare for update
loop() {
  apply(MyComponent, myComponent, _({.value = 42}));
  // At this point, myComponent is in PREPARED state
  // The value hasn't changed yet
}

// Second iteration: Complete the update
loop() {
  // Now myComponent.state.value is 42
  // The component is back in RELEASED state
}
```

## React: Immediate Updates

When you call `react(Type, component, props)`, the entire update happens in a single iteration:

1. Check if the component should update (`should_update()`)
2. Prepare for the update (`will_update()`)
3. Apply the changes (`release()`)
4. Complete the update (`did_update()`)
5. Return to `RELEASED` state

### Example:

```c
loop() {
  react(MyComponent, myComponent, _({.value = 42}));
  // At this point, myComponent.state.value is already 42
  // The component is in RELEASED state
}
```

## Why Split Updates Across Iterations?

There are several important reasons why the EER framework splits updates by default:

### 1. Predictable Data Flow

By splitting updates across iterations, the framework creates a predictable, unidirectional data flow. Components prepare for changes in one iteration and apply them in the next, which prevents cascading updates and race conditions.

### 2. Batched Updates

This approach allows the framework to batch multiple updates together. Several components can prepare for updates in the same iteration, then all apply their changes in the next iteration.

### 3. Performance Optimization

For embedded systems with limited resources, splitting updates allows for better CPU utilization and power management. The system can:
- Prepare all updates in one cycle
- Potentially sleep between cycles
- Apply all updates in the next cycle

### 4. Interrupt Safety

In interrupt-driven systems, having clearly defined update phases makes it easier to ensure that interrupts don't interfere with component updates.

### 5. Debugging and Monitoring

With distinct preparation and application phases, it's easier to debug and monitor the system's behavior. You can inspect the pending changes before they're applied.

## When to Use Each Approach

### Use `apply()` (Default) When:
- You want predictable, batched updates
- You're building a complex system with many interacting components
- You need to optimize for power efficiency
- You want to maintain a clear, unidirectional data flow

### Use `react()` When:
- You need immediate updates for time-critical operations
- You're handling user input that requires immediate feedback
- You're implementing simple components with no dependencies
- You're debugging and need to see changes immediately

## Implementation Details

The implementation of these functions in the EER framework shows the difference:

```c
// apply(): Sets up the update but relies on the next iteration to complete it
#define eer_apply(Type, name, propsValue)                                      \
  if (EER_CONTEXT_UPDATED == eer_land.state.context &&                         \
      (EER_STAGE_RELEASED == name.instance.stage.state.step ||                 \
       EER_STAGE_DEFINED == name.instance.stage.state.step)) {                 \
    Type##_props_t next_props = propsValue;                                    \
    eer_staging(&name.instance, &next_props);                                  \
  } else {                                                                     \
    eer_staging(&name.instance, 0);                                            \
  }

// react(): Forces the component through the entire update cycle immediately
#define eer_react(Type, name, propsValue)                                      \
  {                                                                            \
    Type##_props_t next_props = propsValue;                                    \
    eer_staging(&name.instance, (void *)eer_land.state.context);               \
    if (EER_CONTEXT_BLOCKED != eer_land.state.context) {                       \
      name.instance.stage.state.step = EER_STAGE_REACTING;                     \
      eer_staging(&name.instance, &next_props);                                \
    }                                                                          \
  }
```

## Conclusion

The distinction between `apply()` and `react()` is a fundamental aspect of the EER framework's design philosophy. By providing both options, the framework offers flexibility while encouraging a structured approach to component updates.

For most use cases, `apply()` is the recommended approach as it promotes a clean, predictable data flow. However, `react()` is available when immediate updates are necessary.

Understanding when to use each approach is key to building efficient, maintainable applications with the EER framework.
