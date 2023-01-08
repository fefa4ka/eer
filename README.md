# eer

`eer` is a C99 library for creating microcontroller (MCU) applications using a declarative approach. It allows developers to create simple components for each subsystem and compile a system using these functional components.

`eer` is a library that allows developers to create _real-time applications_ on microcontrollers in a way that is _similar to the way React components are used to build user interfaces._ The library provides a set of components that can be used to build applications, as well as a HAL (_Hardware Abstraction Layer_) that provides a common interface for interacting with the microcontroller's hardware. The library also includes an _event-driven loop_ that allows developers to specify how components should respond to changes in their state or the state of other components.

In general, a declarative programming model can be useful for microcontroller applications because it allows you to **describe the desired output of the system based on its state**, rather than specifying the exact steps to be taken to achieve that output. This can help to simplify the code and make it easier to understand and maintain.

Using functional components can also be a useful approach for microcontroller applications because it allows you to break down the system into smaller, self-contained units of code that can be developed and tested independently. This can help to improve the modularity and maintainability of the code.

## Features

C library provides a set of functions and macros for creating real-time applications using a declarative programming model based on the lifecycle of functional components.

In this model, each component goes through a series of stages during its lifecycle, including **mounting**, **preparation**, **release**, and **unmounting**. Each stage corresponds to a specific set of actions that can be performed by the component. For example, the "_will mount_" and "_did mount_" methods are called when the component is first being added to the system, while the "_should update_" and "_will update_" methods are called when the component is being prepared for an update.

There are **library of components** for common MCU functionality, including GPIO, timers, ADC, buttons, serial communication, PWM, bitbang, SPI, schedulers, and servos.

**Examples of real-time applications**, such as a sensor-dependent motor controller and a shell interface over serial communication

### Benifits

The declarative approach used in this library has several benefits compared to a more traditional, imperative approach to developing MCU applications. Some of the benefits include:

-   Simplicity: By specifying the desired behavior of the system rather than the specific steps to achieve that behavior, you can create simpler, more readable code. This makes it easier to understand and maintain your applications.
-   Modularity: The component-based nature of the library allows you to break down your application into smaller, reusable pieces. This makes it easier to develop and test individual parts of the system, as well as reuse them in other applications.
-   Flexibility: The declarative approach allows you to easily change the behavior of the system by simply updating the component's properties. This allows you to quickly iterate and adapt to changing requirements.
-   Efficiency: The library's internal implementation is optimized for efficient execution, minimizing the overhead of using the declarative approach.

    Overall, the declarative approach used in this library provides a simple, flexible, and efficient way to develop MCU applications.

## Component Lifecycle

Components in the library follow a certain lifecycle, with different stages being triggered depending on the component's current state. The component lifecycle consists of the following stages:

### Mounting Stage

The mounting stage occurs when a component is first initialized. The following methods are called in this stage:

-   `will_mount`: This method is called before the component is mounted. It can be used to perform any initialization tasks that are needed before the component is made available for use.
-   `release`: This method is called to apply any updates to the component's state that were specified in the apply function. It is called after the `will_mount` method and before the `did_mount` method.
-   `did_mount`: This method is called after the component is mounted and its initial state has been applied. It can be used to perform any tasks that need to be done after the component is fully initialized.

### Preparation Stage

The preparation stage occurs before each update to the component's state. The following methods are called in this stage:

-   `should_update`: This method is called to determine whether the component's state should be updated. It should return `true` if the update should be applied, or `false` if it should be skipped.
-   `will_update`: This method is called before the component's state is updated. It can be used to perform any tasks that need to be done before the update is applied.

### Release Stage

The release stage occurs after each update to the component's state. The following methods are called in this stage:

-   `release`: This method is called to apply the update to the component's state. It is called after the `will_update` method and before the `did_update` method.
-   `did_update`: This method is called after the component's state has been updated. It can be used to perform any tasks that need to be done after the update is applied.

### Unmounting Stage

The unmounting stage occurs when a component is no longer needed and is being unmounted. The following method is called in this stage:

-   `did_unmount`: This method is called after the component is unmounted. It can be used to perform any tasks that need to be done after the component is no longer in use.

## Getting Started

To use `eer` in your project, include the library header files and initialize components using the `Component` macro. Components can be updated and applied to the system using the `apply` or `use` macros within the `loop` structure.

```c
#include <eer.h>

/* Initialize component */
Component(component, _({
    .property = value,
    ...
}))

/* Infinity loop */
loop(component, ...) {
    use(component);
    apply(type, component, _({
        .property = value
    }))
}
```

The `use` macro take a component as an argument and causes the component's execution using last setted properties.

The `apply` macro appears to take three arguments: a `type`, a `component`, and a block of properties. The `type` and `component` arguments may specify the type of component and the specific instance of the component to be used, while the properties may include values or settings that are specific to the component. The `apply` macro causes the component's preparation stage with `next_props` to be executed on next iteration of the loop.

### Declarative Usage

The `eer` library includes an event-driven loop that allows developers to specify how components should respond to changes in their state or the state of other components.

The event-loop is structured within a `loop(component, ...) { ... }` macro.

ALso to start the event-loop, developers can start your application description with `ignite(component, ...);` statement and end it with either a `halt(<exit_code>);` statement or a `terminate { ... }` section.

```c
#include <eer.h>

Chip(sys,
   _({
       .on = {
           .boot = print_version,
           .ready = print_shell,
       }
   }),
   _({
       .frequency = {.cpu = CPU_FREQUENCY},
       .sys       = &hw(sys),
   })
);

Clock(clk, &hw(timer), TIMESTAMP);

SPIComputer(spi, _({
   .clock = &clk,
   .io = &hw(gpio)
   ...
}));

void dump_accelerometer (eer_t *imu)
{
    gyro_data.x = eer_state(MMA7455, imu, force.value.x);
    gyro_data.y = eer_state(MMA7455, imu, force.value.y);
    gyro_data.z = eer_state(MMA7455, imu, force.value.z);
}
MMA7455(imu, _({
   .spi             = &spi,
   .chip_select_pin = &hw_pin(A, 3),
   .onChange        = dump_accelerometer,
   ...
}))

Display(indicator, _({
    .property = value,
    ...
}))

/* Infinity loop between ignite and halt */
ignite (sys, clk, ...);

with(spi) {
   with(imu) {
       apply(Display, indicator, _({
           .gyro = &gyro_data
        }));
    }
}

halt(0);

```

### Defining Component

Library provides a simple and straightforward way to define, initialize, and use components within a microcontroller application.

Here is a full implementation of the `MyComponent` component, including the lifecycle methods and any additional functions or methods that you might want to include:

```c
#include <eers.h>

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

You can then use the component in your code like this:

```c
#include <MyComponent.h>

/* Initialize the component with some initial props */
Component(myComponent, _({
  .value = 42
}));

/* Update the component's state using the apply function */
apply(MyComponent, myComponent, _({
  .value = 123
}));

/* Call the component's methods */
MyComponent_reset(&myComponent);
int value = MyComponent_getValue(&myComponent);
```

### HAL API

The C library appears to be a hardware abstraction layer (HAL) that provides a set of functions and structures for working with various hardware components and peripherals on a microcontroller.

The HAL includes several handler structures, such as `eer_sys_handler_t`, `eer_isr_handler_t`, `eer_gpio_handler_t`, `eer_adc_handler_t`, and `eer_timer_handler_t`, which define functions for interacting with the system, handling interrupts, working with GPIOs, using ADC, and working with timers, respectively.

The HAL also includes several enumerations that define constants for various settings and modes, such as `eer_sys_mode`, `eer_communication_mode`, `eer_bit_order`, `eer_io_event`, `eer_io_level`, and `eer_pin_mode`.

## Components Library

-   **GPIO**: This component allows you to interact with the general-purpose input/output (GPIO) pins on your device. You can configure a pin as an input or output, set or read the level (high or low) of an output pin, and register callbacks to be triggered when the level of an input pin changes. This component is often used for simple digital input and output tasks, such as reading the state of a button or controlling an LED.
-   **Timer**: This component allows you to set up a timer that can trigger events at specific intervals. You can use it to create delays, measure elapsed time, or generate periodic events.
-   **ADC**: This component allows you to read analog values from an analog-to-digital converter (ADC) on your device. You can use it to measure analog signals such as the output of a temperature sensor or the position of a potentiometer.
-   **Button**: This component is a specialized version of the GPIO component that is designed specifically for use with buttons. It provides a debouncing mechanism to filter out noise and allows you to register callbacks for press, release, and click events.
-   **Serial**: This component allows you to send and receive data over a serial communication interface, such as a UART or USB. You can use it to communicate with other devices or with a host computer.
-   **UART**: This component is a specialized version of the serial component that is designed specifically for use with universal asynchronous receiver-transmitter (UART) interfaces. It allows you to set the baud rate and other parameters of the UART and provides additional features such as flow control.
-   **PWM**: This component allows you to generate pulse-width modulated (PWM) signals, which can be used to control the brightness of an LED or the speed of a motor, for example.
-   **Bitbang**: This component allows you to use a set of GPIO pins as a simple serial communication interface, such as I2C or SPI.
-   **SPI**: This component allows you to communicate with devices over a serial peripheral interface (SPI) bus. It provides support for multiple slave devices and allows you to set the clock frequency and other parameters of the interface.
-   **Scheduler**: This component allows you to schedule tasks to be run at specific intervals or at specific times. You can use it to create a simple event loop or to manage complex task dependencies.
-   **Servo**: This componet allow you to control servo motors
