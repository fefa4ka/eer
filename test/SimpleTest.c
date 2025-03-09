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

// Hook function to be called after component is mounted
void after_mount_hook(void* data) {
  bool* mounted_ptr = (bool*)data;
  *mounted_ptr = simpleComponent.state.mounted;
  log_info("After mount hook called, mounted = %d", *mounted_ptr);
}

// Hook function to be called after update
void after_update_hook(void* data) {
  int* value_ptr = (int*)data;
  *value_ptr = simpleComponent.state.value;
  log_info("After update hook called, value = %d", *value_ptr);
}

// Global variables to store hook results
bool component_mounted = false;
int updated_value = 0;

// Define a test function to verify the component works
result_t test_simple_component() {
  // Wait for the component to be processed
  test_wait_for_iteration(2);
  
  // Verify the component was mounted
  test_assert(component_mounted == true, 
              "Component should be mounted");
  
  // Verify the value was updated
  test_assert(updated_value == 100, 
              "Value should be updated to 100");
  
  return OK;
}

// Main test function
test(test_simple_component) {
  // Register hooks to capture component state
  test_hook_after_iteration(1, after_mount_hook, &component_mounted);
  test_hook_after_iteration(2, after_update_hook, &updated_value);
  
  // Start the event loop with our component
  loop(simpleComponent) {
    // First iteration: component is mounted
    if (eer_current_iteration == 1) {
      // Update the component with a new value
      apply(SimpleComponent, simpleComponent, _({.value = 100}));
    }
    
    // Exit after two iterations
    if (eer_current_iteration >= 2) {
      break;
    }
  }
}
