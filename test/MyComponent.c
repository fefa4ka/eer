#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
#include "test.h"
#include <stdio.h>

#define MyComponent(instance) eer(MyComponent, instance)
#define MyComponent_new(instance, state)                                       \
  eer_withstate(MyComponent, instance, _(state))
typedef struct {
  int value;
} MyComponent_props_t;

typedef struct {
  int value;
  bool initialized;
  bool updated;
} MyComponent_state_t;

eer_header(MyComponent);

/**
 * Initialize the component state when it's first mounted
 */
WILL_MOUNT(MyComponent) {
  state->initialized = true;
  state->value = props->value;
}

/**
 * Prepare for an update by resetting the updated flag
 */
WILL_UPDATE(MyComponent) { 
  state->updated = false; 
}

/**
 * Always update this component when props change
 */
SHOULD_UPDATE(MyComponent) {
  return true; // Always update by default
}

/**
 * Apply the new props to the component state
 */
RELEASE(MyComponent) { 
  state->value = props->value; 
}

/**
 * Mark the component as updated after the update is complete
 */
DID_UPDATE(MyComponent) { 
  state->updated = true; 
}

/**
 * Handle post-mount operations
 */
DID_MOUNT(MyComponent) {
  // Mount verification done in test
}

/**
 * Handle cleanup when the component is unmounted
 */
DID_UNMOUNT_SKIP(MyComponent);

// Create a component instance with initial state
MyComponent_new(myComponent, _({.value = 42}));

/**
 * Test function that updates the component
 */
test(check_my_component) {
  loop() {
    // Update component with new value
    apply(MyComponent, myComponent, _({.value = 123}));
  }
}

/**
 * Verify the component behaves correctly
 */
result_t check_my_component() {
  // Before the component is mounted
  test_assert(myComponent.state.initialized == false,
              "Component should not be initialized yet");

  test_assert(myComponent.state.value == 42,
              "Initial value should be 42, got %d", myComponent.state.value);
  
  // Give the component time to mount and update
  usleep(5);
  
  // After the component is mounted and updated
  test_assert(myComponent.state.initialized == true,
              "Component should be initialized after mounting");
              
  test_assert(myComponent.state.value == 123,
              "Value should be updated to 123, got %d", myComponent.state.value);

  return OK;
}
