#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
#include "test.h"
#include <stdio.h>

// Define the simplest possible component
typedef struct {
  int value;
} SimpleComponent_props_t;

typedef struct {
  int value;
  bool mounted;
} SimpleComponent_state_t;

eer_header(SimpleComponent);

// Implement only the essential lifecycle methods
WILL_MOUNT(SimpleComponent) {
  state->mounted = true;
  state->value = props->value;
}

// Skip methods we don't need for this simple test
SHOULD_UPDATE_SKIP(SimpleComponent);
WILL_UPDATE_SKIP(SimpleComponent);
DID_MOUNT_SKIP(SimpleComponent);
DID_UPDATE_SKIP(SimpleComponent);
DID_UNMOUNT_SKIP(SimpleComponent);

// Implement release to update state from props
RELEASE(SimpleComponent) {
  state->value = props->value;
}

// Create a component instance with initial props
eer_withprops(SimpleComponent, simpleComponent, _({.value = 42}));

// Define a test function to verify the component works
result_t test_simple_component() {
  // Verify the component was mounted
  test_assert(simpleComponent.state.mounted == true, 
              "Component should be mounted");
  
  // Verify the initial value was set
  test_assert(simpleComponent.state.value == 42, 
              "Initial value should be 42");
  
  // Verify the value was updated
  test_assert(simpleComponent.state.value == 100, 
              "Value should be updated to 100");
  
  return OK;
}

// Main test function
test(test_simple_component) {
  // Start the event loop with our component
  loop() {
    // Update the component with a new value
    apply(SimpleComponent, simpleComponent, _({.value = 100}));
    
    // Exit after one iteration
    break;
  }
}
