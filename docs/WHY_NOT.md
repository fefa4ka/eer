# EER Framework: React-Inspired Patterns for Microcontroller Programming

## Introduction: Bridging Web and Embedded Development

The worlds of web development and embedded systems programming have traditionally existed in separate universes. One is characterized by abundant resources, dynamic memory allocation, and high-level abstractions; the other by strict constraints, real-time requirements, and hardware-level programming. Yet both face similar challenges in managing complexity as applications grow.

The Embedded Event-driven Reactor (EER) framework represents an ambitious attempt to bridge these worlds by bringing React-inspired component architecture to resource-constrained microcontrollers.

## Core Concept: Component-Based Architecture for MCUs

EER introduces a component-based architecture where hardware interactions are encapsulated in self-contained units with clear lifecycle methods:

```c
/* Button component */
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

WILL_MOUNT(Button) {
    state->pressed = false;
    state->last_state = false;
    state->last_change_time = 0;
}

RELEASE(Button) {
    // Component logic implementation
}
```

Components follow a well-defined lifecycle with familiar React-like hooks: `WILL_MOUNT`, `SHOULD_UPDATE`, `WILL_UPDATE`, `RELEASE`, `DID_UPDATE`, and `DID_UNMOUNT`.

## The Initial Skepticism: Can This Really Work?

When first encountered, the EER approach raises legitimate concerns:

1. **Resource Overhead**: Can 8-bit MCUs afford the memory and processing overhead?
2. **Debugging Complexity**: How can developers debug code buried in macro expansions?
3. **Abstraction Cost**: Does the indirection introduce unnecessary complexity?
4. **Practical Benefits**: Do the organizational benefits outweigh the costs?

These concerns seem particularly valid for highly constrained platforms like AVR and STM8 microcontrollers.

## Surprising Reality: It Works on 8-bit MCUs

Despite skepticism, EER was developed and tested on 8-bit AVR and STM8 platforms—some of the most resource-constrained environments in modern embedded development. This achievement challenges the assumption that such architecture patterns are too heavy for limited MCUs.

This success reveals two important aspects of the framework:

1. **Efficient Implementation**: The framework must be remarkably lightweight
2. **Constraint-Driven Design**: Developing directly on constrained platforms forced efficient design from the start

## Smart Engineering: The Preprocessing Approach

A key innovation that makes EER practical is the preprocessing approach to debugging:

```
Original Code → Preprocessor → Expanded Code → Compiler → Debugging
```

By preprocessing the code to expand macros before compilation, developers can debug the actual expanded code rather than struggling with macro indirection. This pragmatic solution transforms what could be a debugging nightmare into a manageable workflow.

## The Context: Real-World Embedded Code Quality

The true value of EER becomes clearer when considering the state of typical embedded code in production:

- Many projects consist of monolithic functions with poor organization
- Global variables and state are scattered throughout the codebase
- Hardware interactions are intermingled with business logic
- Maintenance becomes increasingly difficult as projects grow

In this context, trading some resource efficiency for code organization becomes a compelling proposition—particularly since many embedded projects use nowhere near their MCU's full capacity.

## The Real Calculus: Developer Time vs. MCU Resources

For commercial projects, the economics often favor developer productivity over hardware efficiency:

- Engineering time costs $100+/hour
- MCUs cost $1-10 each
- Well-structured code reduces debug and maintenance time
- Clear component boundaries simplify onboarding and collaboration

As one embedded veteran put it: "I'd gladly trade 10% more flash usage for 50% less debugging time."

## Where EER Makes Sense: A Balanced View

The EER approach offers the most value in these scenarios:

1. **Projects with moderate complexity**: Multiple interacting subsystems benefit from clear separation
2. **Team-developed firmware**: Component boundaries create natural interfaces between developers
3. **Long-lived products**: Maintenance costs often dwarf initial development costs
4. **Systems with multiple variants**: Component reuse across similar products
5. **Projects not pushing hardware limits**: Many modern MCUs have ample headroom

While not suited for every application (especially those optimizing for minimum power or maximum performance), EER addresses real problems in embedded software development.

## The Layered Architecture: A Path Forward

For further development, a modular approach with well-defined interfaces makes the most sense:

```
┌───────────────────────────────┐
│    User Application Code      │
└───────────────┬───────────────┘
                │
┌───────────────▼───────────────┐
│    EER Component Library      │
└───────────────┬───────────────┘
                │
┌───────────────▼───────────────┐
│       EER Core Framework      │
└───────────────┬───────────────┘
                │
┌───────────────▼───────────────┐
│     Hardware Abstraction      │
│      Layer Interface          │
└───────────────┬───────────────┘
                │
┌───────────────▼───────────────┐
│  Platform-Specific HAL Impl.  │
└───────────────────────────────┘
```

This approach allows users to include only the parts they need while providing a clean separation between the component model and hardware-specific implementations.

## Conclusion: Unexpected Innovation

The EER framework challenges conventional wisdom about embedded software architecture. By bringing React-inspired patterns to microcontrollers—even 8-bit ones—it offers a fresh approach to managing complexity in firmware development.

While not appropriate for every embedded project, EER addresses a significant gap in the embedded development ecosystem: the need for better code organization and component reuse in an increasingly complex firmware landscape.

The most successful embedded frameworks find the right balance between structure and simplicity, between abstraction and control. EER demonstrates that with careful implementation, modern software architecture patterns can be adapted even for the most constrained computing environments.

In a world where "it just works" often masks unreadable, unmaintainable code, frameworks that bring disciplined structure to embedded development deserve serious consideration—even if they make embedded purists uncomfortable at first glance.
