# React-like Patterns for Microcontroller Programming

## Bridging Two Worlds

It's strange, because web development and programming microcontrollers seem to be very different from one another. One is all about crafting slick user interfaces in the comfort of a web browser, with plenty of room to breathe — think dynamic memory, high-level abstractions, and resources that feel practically infinite. The other one? In this world of strict constraints, you're tinkering at the hardware level, real-time deadlines are the rule, and every byte of memory counts.

At their core, both web and MCU programming face one major problem: things getting too complicated.

If you've ever worked on firmware for a microcontroller, you've probably felt the pain. As your codebase grows, it starts to look like a plate of spaghetti someone dropped on the floor. Bits of logic for controlling peripherals end up scattered all over the place. Interrupt handlers and state changes show up in the most unlikely places in your code. How we love to make a mess, don't we? Enough to remind you of your nightmares about debugging and trying to integrate a new feature into the system? Pulling one thread without ripping the entire sweater can be like that.

But what if — crazy as it sounds — we took some of the beautiful ideas of React, that groundbreaking library for web development, and brought them into MCU programming? React embraces modularity, thinking about what to do, not how to do it, and handling state in an explicit, predictable way.

This article will explore just that: how these architectural patterns of React component-based design, state-driven updates, and lifecycle management might look and feel in the strict resource-constrained world of microcontrollers; how such ideas can be translated by using clever tricks such as C macros and compile-time optimizations to make sense for hardware in firmware development.

## Why React for MCUs?

It shouldn't be a surprise that its core principles have addressed many of the universal problems in software architecture and hence could find applications far from their domain of origin. The core of React is all about organizing complexity. In this chaotic world of microcontrollers where everything, from toggling LEDs to managing UART communication, seems to involve hard-to-follow manipulations of registers with state variables scattered here and there, this emphasis on simplicity has huge appeal.

That means, basically, that we don't have to port React itself but can work out the main principles of React using embedded development native means. By using C macros together with compile-time code generation carefully, one can get a React-like component architecture without runtime overhead. It is not just a theoretical thought; I have managed to apply it in real production systems, from simple LED controllers up to complex industrial automation equipment.

## Adapting the Core Concepts

Limited memory, no garbage collection, real-time requirements, direct communication with hardware are some of the features that define the resource-constrained environment of the MCU. The fundamental ideas of React —componentization, state management, and unidirectional data flow — can be successfully reapplied despite these limitations.

### Component-based design

Reusable components are a rewarding aspect of using React to create user interfaces. They are separate entities with distinct states, logics, and behaviors. Now consider using this concept in the design of MCUs. What if we saw hardware components like motors, timers, buttons, and LEDs as separate "components"? Remove the intricate low-level elements to allow for modularity and reusability across projects.

In React, components are dynamic and interactive, typically responding to user actions through the virtual DOM. In MCUs, on the other hand, our components are static and fully deterministic and can be modeled with a structured block in C that can store state and props, where props represent well-defined interfaces for interaction.

```c
/* Button component */

typedef struct {
    eer_gpio_handler_t *io;
    void *              pin;

    enum {
        BUTTON_PUSH,  /* Button pressed only when pushed */
        BUTTON_TOGGLE /* Pressed state change after every push */
    } type;


    struct Clock_time *clock;           /* Timestamp for bounce filtering */
    int                bounce_delay_ms; /* Debounce delay in milliseconds */

    struct {
        void (*press)(eer_t *instance);
        void (*release)(eer_t *instance);
        void (*toggle)(eer_t *instance); /* When pressed state changes */
    } on;
} Button_props_t;

typedef struct {
    union {
        struct {
            bool level : 1;
            bool pressed : 1; /* Is button pressed */
        };
        uint8_t flags;
    };
    uint16_t tick; /* Timestamp when pin state changed */
} Button_state_t;
```

For example there are simple IO and Button components implementations.

By thinking about embedded peripherals through the lens of "components," MCU development becomes not only more manageable, but also more elegant — bringing a touch of software engineering finesse to the often daunting world of hardware.

### State management and synchronization

React emphasizes a single source of truth for state, ensuring that updates flow predictably and declaratively through the system. On an MCU, state could represent the configuration or behavior of hardware peripherals — such as whether a motor is running, an LED is on, or a temperature sensor has reached a threshold. By organizing these states into central or local "state containers," we can track peripheral states systematically, avoiding errors from scattered and unsynchronized state changes.

Global State Allocation: Unlike the dynamic state objects of React, we allocate state globally at compile time. This guarantees having full control over memory usage.

Deterministic Updates: Well-designed update cycles manage state changes. This ensures that components remain consistent with their underlying hardware, preventing problems like race conditions or inconsistent states.

### Declarative programming

Traditional embedded programming relies heavily on imperative "how-to" code: explicitly writing out each pin configuration, state update, and peripheral interaction. A React-inspired approach allows developers to focus on "what" the system should do, leaving the "how" to abstractions.

```c
Clock(clk, &hw(timer), TIMESTAMP);

/* Button component with full lifecycle */
pin_t enter_pin = hw_pin(ENTER_PORT, ENTER_PIN);
Button(enter, _({
    .io = &hw(gpio),
    .pin = &enter_pin,
    .clock = &clk.state.time,
    .type = BUTTON_PUSH_PULLUP,
    .bounce_delay_ms = 100,
    .on = {
        .press = handle_press,
        .release = handle_release
    }
}));

/* Main application loop */
/* Components automatically manages its lifecycle */
loop(clk, enter) {
    /* React to state changes */
    if (Button_is_pressed(enter)) {
        /* Handle pressed state */
    }
}
```

More advanced example of simple shell implementation.

### One-Way Data Flow

React enforces a unidirectional data flow, ensuring that changes propagate predictably from the source of truth (state) to the rendered interface. On MCUs, this principle can manage interactions between input peripherals (e.g., sensor data) and output peripherals (e.g., motors or displays), preventing chaotic feedback loops or race conditions.

### Component Lifecycle

Hardware accessories, sensors, or logical units function similarly to React components in this method; each has a defined lifetime, ranging from initialization (mounting) to state changes (updating) and cleanup (unmounting). We replicate the elegance of React while maintaining the code's efficiency and minimal weight, which is precisely what embedded systems require, by using C macros to build a declarative API.

```c
#include "MyComponent.h"

WILL_MOUNT(MyComponent) {
    state->initialized = true;
}

SHOULD_UPDATE(MyComponent) {
    return props->value != next_props->value;
}

WILL_UPDATE(MyComponent) {
    state->updated = false;
}

RELEASE(MyComponent) {
    state->value = props->value;
}

DID_UPDATE(MyComponent) {
    state->updated = true;
    printf("MyComponent updated with value %d\n", state->value);
}

DID_MOUNT(MyComponent) {
    printf("MyComponent mounted with value %d\n", state->value);
}
```

This pattern keeps code organized, reusable, and debug-friendly with clear states like DEFINED → RELEASED → PREPARED → RELEASED → … and hooks for each step.

#### Initialization Phase

- **DEFINED**: Component declared with initial properties
- **WILL_MOUNT**: Prepare component for first use (e.g., initialize hardware)
- **RELEASE**: Apply initial state
- **DID_MOUNT**: Component is ready for operation

#### Update Phase

- **RELEASED**: Component is ready for new update lifecycle
- **SHOULD_UPDATE**: Determine if component needs updating
- **WILL_MOUNT**: Prepare for state/property changes
- **PREPARED**: Component prepared to apply changes
- **RELEASE**: Apply changes
- **DID_UPDATE**: Post-update operations

This would mean that management of component states and updates in embedded devices can be really straightforward with React lifecycle patterns for MCU programming. The beautiful philosophy of React, mixed with the unique challenges of embedded programming, will result in code that is easier to maintain but above all, much more reliable.

The implementation shows that complex web development methodologies can be successfully adapted for resource-constrained environments while keeping the code clean and the system reliable.

## References

- Embedded Event-based React: EER / Components / Applications
- React Library for Web Development: [React Official Website](https://reactjs.org)

## To be continued

I intend to delve deeper into the development framework in next articles:

- **Implementation EER Framework** — Dive deeper into designing a React-inspired development framework for MCUs
- **Event-Driven Architecture** — Explores how to abstract away low-level interrupt logic using event-driven patterns.
- **Real-Time Considerations** — How can we make sure that a system inspired by React behaves predictably and promptly?
- **Writing Applications** — This section demonstrates how to apply the concepts and techniques from previous articles in the series to real-world scenarios.
- **Tooling and Debugging** — Covers tools and techniques for visualizing state changes, hardware states, component simulations, and workflow improvements.
