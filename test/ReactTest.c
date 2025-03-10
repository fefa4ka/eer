/**
 * React Test
 *
 * This test verifies the behavior of the react macro for forced updates.
 */

#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
#include "test.h"
#include <stdio.h>
#include <unistd.h>

/* Define a component that normally rejects updates */
typedef struct {
  int value;
} ReactComponent_props_t;

typedef struct {
  int value;
  int update_count;
  int forced_update_count;
} ReactComponent_state_t;

eer_header(ReactComponent);

WILL_MOUNT(ReactComponent) {
  state->value = props->value;
  state->update_count = 0;
  state->forced_update_count = 0;
  log_info("ReactComponent mounted with value: %d", state->value);
}

// This component only updates when the value is even
SHOULD_UPDATE(ReactComponent) {
  bool should_update = next_props->value % 2 == 0;
  log_info("SHOULD_UPDATE called with value %d, returning %s", 
           next_props->value, should_update ? "true" : "false");
  return should_update;
}

WILL_UPDATE(ReactComponent) {
  log_info("WILL_UPDATE called, preparing to update from %d to %d", 
           state->value, next_props->value);
}

RELEASE(ReactComponent) {
  log_info("RELEASE called, updating value from %d to %d (update_count: %d)", 
           state->value, props->value, state->update_count);
  state->value = props->value;
  state->update_count++;
  
  // Check if this was a forced update (react bypasses should_update)
  if (props->value % 2 != 0) {
    state->forced_update_count++;
    log_info("Forced update detected (odd value: %d, forced: %d)", 
             props->value, state->forced_update_count);
  }
}

DID_UPDATE(ReactComponent) {
  log_info("DID_UPDATE called, value is now %d (update #%d, forced: %d)", 
           state->value, state->update_count, state->forced_update_count);
}

DID_MOUNT_SKIP(ReactComponent);
DID_UNMOUNT_SKIP(ReactComponent);

/* Create component instance */
eer_withprops(ReactComponent, reactComponent, _({.value = 0}));

/* Global variables to store test results */
int normal_updates = 0;
int forced_updates = 0;

/* Hook function to capture component state */
void after_react_update(void *data) {
  int *updates = (int *)data;
  normal_updates = reactComponent.state.update_count;
  forced_updates = reactComponent.state.forced_update_count;
  *updates = normal_updates;
  log_info("Total updates: %d (normal: %d, forced: %d)", 
           *updates, normal_updates, forced_updates);
}

/* Test react for forced updates */
test(test_react_updates) {
  int total_updates = 0;
  
  // Register hook to capture update counts
  test_hook_after_iteration(           6, after_react_update, &total_updates);
  
  // Start loop with the component
  loop() {
    // First iteration: normal update with even value (should succeed)
    if (eer_current_iteration == 1) {
      log_info("Iteration 1: Normal update with even value (2)");
      apply(ReactComponent, reactComponent, _({.value = 2}));
    }
    
    // Second iteration: normal update with odd value (should be rejected)
    else if (eer_current_iteration == 2) {
      log_info("Iteration 2: Normal update with odd value (3)");
      apply(ReactComponent, reactComponent, _({.value = 3}));
    }
    
    // Third iteration: forced update with odd value (should succeed despite should_update)
    else if (eer_current_iteration == 3) {
      log_info("Iteration 3: Forced update with odd value (5)");
      react(ReactComponent, reactComponent, _({.value = 5}));
    }
    
    // Fourth iteration: normal update with even value (should succeed)
    else if (eer_current_iteration == 4) {
      log_info("Iteration 4: Normal update with even value (6)");
      apply(ReactComponent, reactComponent, _({.value = 6}));
    }
    
    // Fifth iteration: forced update with odd value (should succeed)
    else if (eer_current_iteration == 5) {
      log_info("Iteration 5: Forced update with odd value (7)");
      react(ReactComponent, reactComponent, _({.value = 7}));
    }
    
    // Exit after 5 iterations
    if (eer_current_iteration >= 5) {
      log_info("Test complete, exiting loop");
      eer_land.state.unmounted = true;
    }
  }
}

/* Verification function */
result_t test_react_updates() {
  // Wait for test to complete
  test_wait_for_iteration(6);
  
  // Verify the component was updated correctly
  test_assert(forced_updates == 2, 
              "Component should have 2 forced updates, got %d", 
              forced_updates);
  test_assert(normal_updates == 4, 
              "Component should have 4 total updates, got %d", 
              normal_updates);
  
  
  // Verify the final value
  test_assert(reactComponent.state.value == 7, 
              "Final value should be 7, got %d", 
              reactComponent.state.value);
  
  return OK;
}
