/**
 * Ignite/Terminate/Halt Test
 *
 * This test verifies the behavior of the ignite/terminate/halt approach
 * with various exit conditions.
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
  bool should_exit;
} IgniteComponent_props_t;

typedef struct {
  int value;
  int update_count;
  bool should_exit;
} IgniteComponent_state_t;

eer_header(IgniteComponent);

WILL_MOUNT(IgniteComponent) {
  state->value = props->value;
  state->update_count = 0;
  state->should_exit = props->should_exit;
  log_info("IgniteComponent mounted with value: %d", state->value);
}

SHOULD_UPDATE_SKIP(IgniteComponent);
WILL_UPDATE_SKIP(IgniteComponent);

RELEASE(IgniteComponent) {
  state->value = props->value;
  state->should_exit = props->should_exit;
  state->update_count++;
  log_info("IgniteComponent updated to value: %d (update #%d)", 
           state->value, state->update_count);
}

DID_UPDATE_SKIP(IgniteComponent);
DID_MOUNT_SKIP(IgniteComponent);
DID_UNMOUNT_SKIP(IgniteComponent);

/* Create component instance */
eer_withprops(IgniteComponent, igniteComponent, _({.value = 10, .should_exit = false}));

/* Global variables to store test results */
int ignite_updates = 0;
int exit_code = -1;

/* Hook function to capture component state */
void after_ignite_update(void *data) {
  int *updates = (int *)data;
  *updates = igniteComponent.state.update_count;
  log_info("Ignite component updates: %d", *updates);
}

/* Test function that uses ignite/terminate/halt */
int run_ignite_test() {
  // Initialize the component
  ignite();
  
  // First iteration - update component
  apply(IgniteComponent, igniteComponent, 
        _({.value = igniteComponent.state.value + 5, .should_exit = false}));
  
  // Use terminate to restart the loop
  terminate;
  
  // Second iteration - update component again
  apply(IgniteComponent, igniteComponent, 
        _({.value = igniteComponent.state.value + 10, .should_exit = false}));
  
  // Use terminate to restart the loop
  terminate;
  
  // Third iteration - set exit flag and update
  apply(IgniteComponent, igniteComponent, 
        _({.value = igniteComponent.state.value + 15, .should_exit = true}));
  
  // Check if we should exit
  if (igniteComponent.state.should_exit) {
    eer_land.state.unmounted = true;
    return 42; // Return a specific exit code for testing
  }
  
  // This should not be reached if exit flag is set
  halt(0);
}

/* Test the ignite/terminate/halt approach */
test(test_ignite_halt) {
  // Register hook to capture update count
  test_hook_after_iteration(3, after_ignite_update, &ignite_updates);
  
  // Run the test function that uses ignite/terminate/halt
  exit_code = run_ignite_test();
  
  // Wait for a bit to ensure hooks are executed
  usleep(10000);
}

/* Verification function */
result_t test_ignite_halt() {
  // Wait for test to complete
  test_wait_for_iteration(4);
  
  // Verify the component was updated correctly
  test_assert(ignite_updates == 3, 
              "Ignite component should have 3 updates, got %d", 
              ignite_updates);
  
  test_assert(igniteComponent.state.value == 40, 
              "Ignite component value should be 40, got %d", 
              igniteComponent.state.value);
  
  // Verify the exit code
  test_assert(exit_code == 42, 
              "Exit code should be 42, got %d", 
              exit_code);
  
  return OK;
}
