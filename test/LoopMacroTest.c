/**
 * Single Component Loop Test
 *
 * This test verifies the behavior of the loop macro with a single component.
 */

#include "test.h"
#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
#include <stdio.h>
#include <unistd.h>

/* Define a simple component for testing */
typedef struct {
  int value;
} SimpleLoopComponent_props_t;

typedef struct {
  int value;
  int update_count;
} SimpleLoopComponent_state_t;

eer_header(SimpleLoopComponent);

WILL_MOUNT(SimpleLoopComponent) {
  state->value = props->value;
  state->update_count = 0;
  log_info("SimpleLoopComponent mounted with value: %d", state->value);
}

SHOULD_UPDATE_SKIP(SimpleLoopComponent);
WILL_UPDATE_SKIP(SimpleLoopComponent);

RELEASE(SimpleLoopComponent) {
  state->value = props->value;
  state->update_count++;
  log_info("SimpleLoopComponent updated to value: %d (update #%d)", 
           state->value, state->update_count);
}

DID_UPDATE_SKIP(SimpleLoopComponent);
DID_MOUNT_SKIP(SimpleLoopComponent);
DID_UNMOUNT_SKIP(SimpleLoopComponent);

/* Create component instance */
eer_withprops(SimpleLoopComponent, singleComponent, _({.value = 10}));

/* Global variable to store test results */
int single_component_updates = 0;

/* Hook function to capture component state */
void after_single_update(void *data) {
  int *updates = (int *)data;
  *updates = singleComponent.state.update_count;
  log_info("Single component updates: %d", *updates);
}

/* Test single component loop */
test(test_single_component_loop) {
  // Register hook to capture update count
  test_hook_after_iteration(3, after_single_update, &single_component_updates);
  
  // Start loop with a single component
  loop(singleComponent) {
    // Update component
    apply(SimpleLoopComponent, singleComponent, 
          _({.value = singleComponent.state.value + 5}));
    
    // Exit after 3 iterations
    if (eer_current_iteration >= 3) {
      eer_land.state.unmounted = true;
    }
  }
}

/* Verification function */
result_t test_single_component_loop() {
  // Wait for test to complete
  test_wait_for_iteration(4);
  
  // Verify the component was updated correctly
  test_assert(single_component_updates == 3, 
              "Single component should have 3 updates, got %d", 
              single_component_updates);
  
  test_assert(singleComponent.state.value == 25, 
              "Single component value should be 25, got %d", 
              singleComponent.state.value);
  
  return OK;
}
