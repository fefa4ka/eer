# Implementation of the EER Framework

## From React Concepts to Embedded Reality

The Embedded Event-driven Reactor (EER) framework translates React's component-based architecture to the resource-constrained world of microcontrollers. This article explores the technical implementation details that make this possible, showing how we can achieve React-like patterns without sacrificing performance or memory efficiency.

## Core Architecture

### Component Structure

At the heart of the EER framework is the component structure. Unlike React's JavaScript objects, EER components are implemented as C structures with a carefully designed memory layout:

```c
typedef struct eer {
  union eer_stage stage;  // Component lifecycle state

  // Lifecycle method pointers
  void (*will_mount)(void *instance, void *next_props);
  bool (*should_update)(void *instance, void *next_props);
  void (*will_update)(void *instance, void *next_props);
  void (*release)(void *instance);
  void (*did_mount)(void *instance);
  void (*did_update)(void *instance);
  void (*did_unmount)(void *instance);

#ifdef PROFILING
  PROFILING_STRUCT  // Optional profiling data
#endif
} eer_t;
```

Each component consists of:

1. **Core Instance**: The `eer_t` structure containing lifecycle state and method pointers
2. **Props**: Input parameters that control the component's behavior
3. **State**: Internal data that can change over time

This structure is generated at compile time using macros:

```c
typedef struct MyComponent {
    eer_t          instance;  // Core component structure
    MyComponent_props_t props;  // Component properties
    MyComponent_state_t state;  // Component state
} MyComponent_t;
```

### Memory Efficiency

Unlike React's dynamic memory allocation, EER components are statically allocated at compile time. This approach has several advantages for embedded systems:

1. **Predictable Memory Usage**: The exact memory footprint is known at compile time
2. **No Heap Fragmentation**: Static allocation eliminates heap fragmentation issues
3. **Faster Access**: Direct structure access is faster than dynamic property lookups

For a typical component, the memory overhead is minimal:
- `eer_t` structure: ~20-30 bytes (depending on architecture)
- Props and state: Varies based on component needs, but typically 4-32 bytes each

### The Staging Process

The core of the EER framework is the `eer_staging` function, which manages component lifecycle transitions:

```c
enum eer_context eer_staging(eer_t *instance, void *next_props)
{
    // Determine if next_props is actually a context flag
    uintptr_t context = (uintptr_t)next_props;

    // Handle different context flags and component lifecycle transitions
    if (EER_STAGE_RELEASED == instance->stage.state.step) {
        /* Normal update path */
        if(!instance->should_update(instance, next_props)) {
            return EER_CONTEXT_SAME; // Skip update if should_update returns false
        }
        instance->stage.state.step = EER_STAGE_PREPARED;
        instance->will_update(instance, next_props);
    } else if (EER_STAGE_REACTING == instance->stage.state.step) {
        /* Forced update path (react) */
        instance->stage.state.step = EER_STAGE_PREPARED;
        instance->will_update(instance, next_props);
        instance->stage.state.step = EER_STAGE_RELEASED;
        instance->release(instance);
        instance->did_update(instance);
    } else if (EER_STAGE_PREPARED == instance->stage.state.step) {
        /* Complete the update process */
        instance->stage.state.step = EER_STAGE_RELEASED;
        instance->release(instance);
        instance->did_update(instance);
    } else if (EER_STAGE_DEFINED == instance->stage.state.step) {
        /* Initial mounting process */
        instance->will_mount(instance, next_props);
        instance->release(instance);
        instance->did_mount(instance);
        instance->stage.state.step = EER_STAGE_RELEASED;
    } else if (EER_STAGE_UNMOUNTED == instance->stage.state.step) {
        /* Unmounting process */
        instance->stage.state.step = EER_STAGE_BLOCKED;
        instance->did_unmount(instance);
    }

    return EER_CONTEXT_UPDATED;
}
```

This function handles all transitions between component lifecycle states, ensuring that components move through their lifecycle in a predictable way.

## Macro Magic: Creating a Declarative API

The EER framework uses C preprocessor macros extensively to create a declarative API that resembles React's JSX syntax. This approach allows developers to write clean, expressive code while the preprocessor generates efficient C code.

### Component Definition Macros

The `eer_header` macro generates the necessary type definitions and function prototypes for a component:

```c
#define eer_header(Type)                                                       \
    typedef struct Type {                                                      \
        eer_t          instance;                                               \
        Type##_props_t props;                                                  \
        Type##_state_t state;                                                  \
    } Type##_t;                                                                \
    void Type##_will_mount(void *instance, void *next_props);                  \
    bool Type##_should_update(void *instance, void *next_props);               \
    void Type##_will_update(void *instance, void *next_props);                 \
    void Type##_release(void *instance);                                       \
    void Type##_did_mount(void *instance);                                     \
    void Type##_did_unmount(void *instance);                                   \
    void Type##_did_update(void *instance)
```

### Component Creation Macros

Components are created using macros like `eer_withprops`:

```c
#define eer_withprops(Type, instance_name, instance_props)                     \
    Type##_t instance_name = {                                                 \
        .instance = eer_define_component(Type, instance_name),                 \
        .props    = instance_props,                                            \
    }
```

The `eer_define_component` macro initializes the component's instance structure:

```c
#define eer_define_component(Type, name)                                       \
    {                                                                          \
        .stage = {.state = {.step = EER_STAGE_DEFINED, .updated = false,       \
                           .context = EER_CONTEXT_SAME, .raise_on = 0}},       \
        .will_mount = Type##_will_mount,                                       \
        .should_update = Type##_should_update,                                 \
        .will_update = Type##_will_update, .release = Type##_release,          \
        .did_mount = Type##_did_mount, .did_update = Type##_did_update,        \
        .did_unmount = Type##_did_unmount                                      \
    }
```

### Lifecycle Method Macros

Lifecycle methods are defined using macros that generate the appropriate function definitions:

```c
#define eer_will_mount(Type)                                                   \
    eer_updatecycle_header(Type, will_mount, void);                            \
    void Type##_will_mount(void *instance, void *next_props_ptr)               \
    {                                                                          \
        eer_self(Type, instance);                                              \
        if (!next_props_ptr)                                                   \
            next_props_ptr = &self->props;                                     \
        eer_selfnext(Type, instance);                                          \
        if (&self->props != next_props)                                        \
            self->props = *next_props;                                         \
        eer_lifecycle_prepare(Type, instance, will_mount);                     \
        Type##_inline_will_mount(&self->instance, &self->props, &self->state,  \
                                 next_props);                                  \
        eer_lifecycle_finish(Type, instance, will_mount);                      \
    }                                                                          \
    eer_updatecycle_header(Type, will_mount, void)
```

These macros handle the boilerplate code for accessing component props and state, allowing developers to focus on the component's logic.

## Event Loop Implementation

The EER framework provides two approaches to creating an event loop: the `loop` macro and the `ignite`/`terminate`/`halt` macros.

### The Loop Macro

The `loop` macro creates a for-loop that iterates until the component is unmounted:

```c
#define eer_loop(...)                                                          \
  goto eer_boot;                                                               \
  eer_boot:                                                                    \
  eer_while(__VA_ARGS__)

#define eer_while(...)                                                         \
  for (union eer_land eer_land =                                               \
           {.state = {.context = IF_ELSE(HAS_ARGS(__VA_ARGS__))(               \
                          (EVAL(MAP(__eer_init, __VA_ARGS__))                  \
                               EER_CONTEXT_SAME))(EER_CONTEXT_UPDATED),        \
                      .finished = false,                                       \
                      .unmounted = false,                                      \
                      .step = 0}};                                             \
       !eer_land.state.unmounted && eer_land.state.context;                    \
       eer_land.state.context = IF_ELSE(HAS_ARGS(__VA_ARGS__))((EVAL(MAP(      \
           __eer_init, __VA_ARGS__)) EER_CONTEXT_SAME))(EER_CONTEXT_UPDATED))
```

This macro initializes the components passed to it and creates a loop that continues until the component is unmounted.

### The Ignite/Terminate/Halt Macros

The `ignite` macro creates a goto-based loop:

```c
#define eer_init(...)                                                          \
  union eer_land __attribute__((unused)) eer_land;                             \
  eer_land.flags = 0;                                                          \
  goto eer_boot;                                                               \
  eer_boot:                                                                    \
  eer_land.state.context = IF_ELSE(HAS_ARGS(__VA_ARGS__))((EVAL(               \
      MAP(__eer_init, __VA_ARGS__)) EER_CONTEXT_UPDATED))(EER_CONTEXT_UPDATED)
```

The `terminate` and `halt` macros control the flow of the loop:

```c
#define eer_terminate                                                          \
  if (!eer_land.state.unmounted)                                               \
    goto eer_boot;
#define eer_halt(code)                                                         \
  if (!eer_land.state.unmounted)                                               \
    goto eer_boot;                                                             \
  {                                                                            \
    return code;                                                               \
  }
```

## Apply vs React: Two Update Patterns

The EER framework provides two methods for updating components: `apply` and `react`.

### Apply: Two-Phase Updates

The `apply` macro implements a two-phase update process:

```c
#define eer_apply(Type, name, propsValue)                                      \
  if (EER_CONTEXT_UPDATED == eer_land.state.context &&                         \
      (EER_STAGE_RELEASED == name.instance.stage.state.step ||                 \
       EER_STAGE_DEFINED == name.instance.stage.state.step)) {                 \
    eer_lifecycle_prepare(Type, &name, next_props);                            \
    Type##_props_t next_props = propsValue;                                    \
    eer_lifecycle_finish(Type, &name, next_props);                             \
    eer_staging(&name.instance, &next_props);                                  \
  } else {                                                                     \
    eer_staging(&name.instance, 0);                                            \
  }
```

This approach splits the update across two loop iterations:
1. First iteration: Prepare for the update
2. Second iteration: Apply the changes

### React: Immediate Updates

The `react` macro implements an immediate, single-phase update:

```c
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

This approach forces all lifecycle methods to execute immediately in a single iteration.

## Optimizations for Embedded Systems

The EER framework includes several optimizations for embedded systems:

### Bit-Packed State Flags

Component state flags are packed into bit fields to minimize memory usage:

```c
union eer_stage {
  struct {
    enum {
      EER_STAGE_BLOCKED,
      EER_STAGE_RELEASED,
      EER_STAGE_DEFINED,
      EER_STAGE_REACTING,
      EER_STAGE_PREPARED,
      EER_STAGE_UNMOUNTED
    } step : 3;
    bool updated : 1;
    enum eer_context context : 2;
    uint8_t raise_on : 2;
  } state;
  uint8_t flags;
};
```

This union allows efficient access to individual flags while also allowing the entire state to be manipulated as a single byte.

### Compile-Time Component Registration

Components are registered at compile time, eliminating the need for dynamic component registration:

```c
eer_withprops(MyComponent, myComponent, _({
  .value = 42
}));

loop(myComponent) {
  // Component is automatically initialized and updated
}
```

### Inline Lifecycle Methods

Lifecycle methods are defined as inline functions to reduce function call overhead:

```c
#if __GNUC__
    #define eer_lifecycle_header(Type, stage)                                  \
        static inline void Type##_inline_##stage(                              \
            eer_t *self, Type##_props_t *props, Type##_state_t *state)
#else
    #define eer_lifecycle_header(Type, stage)                                  \
        inline void Type##_inline_##stage(eer_t *self, Type##_props_t *props,  \
                                          Type##_state_t *state)
#endif
```

### Optional Profiling

The framework includes optional profiling capabilities that can be enabled with the `PROFILING` macro:

```c
#ifdef PROFILING
#include "profiler.h"
#endif

// In the eer_t structure
#ifdef PROFILING
  PROFILING_STRUCT
#endif
```

This allows developers to measure performance without adding overhead to production builds.

## Real-World Example: Button Component

Let's look at a real-world example of a button component implemented with the EER framework:

```c
/* Button component definition */
typedef struct {
    pin_t pin;
    int debounce_ms;
    void (*on_press)(void);
    void (*on_release)(void);
} Button_props_t;

typedef struct {
    bool pressed;
    bool last_state;
    uint32_t last_change_time;
} Button_state_t;

eer_header(Button);

/* Button component implementation */
WILL_MOUNT(Button) {
    state->pressed = false;
    state->last_state = false;
    state->last_change_time = 0;
}

RELEASE(Button) {
    // Read the current pin state
    bool current_state = read_pin(props->pin);
    
    // Debounce logic
    uint32_t current_time = get_system_time_ms();
    if (current_state != state->last_state && 
        (current_time - state->last_change_time) > props->debounce_ms) {
        
        state->last_state = current_state;
        state->last_change_time = current_time;
        
        // Update pressed state and call callbacks
        if (current_state) {
            state->pressed = true;
            if (props->on_press) {
                props->on_press();
            }
        } else {
            state->pressed = false;
            if (props->on_release) {
                props->on_release();
            }
        }
    }
}
```

This component encapsulates all the logic for a button, including debouncing, state tracking, and event callbacks.

## Conclusion

The EER framework demonstrates that React's component-based architecture can be successfully adapted for embedded systems. By using C preprocessor macros and careful memory management, we can create a declarative API that resembles React while maintaining the performance and memory efficiency required for microcontrollers.

Key takeaways:
1. **Static Allocation**: Components are statically allocated at compile time
2. **Predictable Lifecycle**: Components follow a well-defined lifecycle
3. **Declarative API**: Macros create a clean, expressive API
4. **Memory Efficiency**: Bit-packed state flags and inline functions minimize overhead
5. **Two Update Patterns**: `apply` for predictable updates, `react` for immediate updates

The EER framework shows that even in the resource-constrained world of embedded systems, we can apply modern software architecture patterns to create more maintainable and reliable code.
