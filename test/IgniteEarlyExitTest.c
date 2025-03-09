/**
 * Ignite Early Exit Test
 *
 * This test verifies the behavior of the ignite/halt approach
 * with an early exit condition.
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
  int max_iterations;
} EarlyExitComponent_props_t;

typedef struct {
  int value;
  int update_count;
  int max_iterations;
} EarlyExitComponent_state_t;

eer_header(EarlyExitComponent);

WILL_MOUNT(EarlyExitComponent) {
  state->value = props->value;
  state->update_count = 0;
  state->max_iterations = props->max_iterations;
  log_info("EarlyExitComponent mounted with value: %d, max iterations: %d", 
           state->value, state->max_iterations);
}

SHOULD_UPDATE_SKIP(EarlyExitComponent);
WILL_UPDATE_SKIP(EarlyExitComponent);

RELEASE(EarlyExitComponent) {
  state->value = props->value;
  state->max_iterations = props->max_iterations;
  state->update_count++;
  log_info("EarlyExitComponent updated to value: %d (update #%d)", 
           state->value, state->update_count);
}

DID_UPDATE_SKIP(EarlyExitComponent);
DID_MOUNT_SKIP(EarlyExitComponent);
DID_UNMOUNT_SKIP(EarlyExitComponent);

/* Create component instance */
eer_withprops(EarlyExitComponent, earlyExitComponent, _({.value = 5, .max_iterations = 2}));

/* Global variables to store test results */
int early_exit_updates = 0;
int early_exit_code = -1;

/* Hook function to capture component state */
void after_early_exit_update(void *data) {
  int *updates = (int *)data;
  *updates = earlyExitComponent.state.update_count;
  log_info("Early exit component updates: %d", *updates);
}

/* Test function that uses ignite/halt with early exit */
int run_early_exit_test() {
  // Initialize the component
  ignite(earlyExitComponent);
  
  // Update component
  apply(EarlyExitComponent, earlyExitComponent, 
        _({.value = earlyExitComponent.state.value + 5, 
           .max_iterations = earlyExitComponent.state.max_iterations}));
  
  // Check if we should exit early
  if (earlyExitComponent.state.update_count >= earlyExitComponent.state.max_iterations) {
    log_info("Early exit condition met after %d updates", 
             earlyExitComponent.state.update_count);
    eer_land.state.unmounted = true;
    return 100; // Return a specific exit code for testing
  }
  
  // Continue the loop if max iterations not reached
  halt(0);
}

/* Test the ignite/halt approach with early exit */
test(test_early_exit) {
  // Register hook to capture update count
  test_hook_after_iteration(2, after_early_exit_update, &early_exit_updates);
  
  // Run the test function that uses ignite/halt with early exit
  early_exit_code = run_early_exit_test();
  
  // Wait for a bit to ensure hooks are executed
  usleep(10000);
}

/* Verification function */
result_t test_early_exit() {
  // Wait for test to complete
  test_wait_for_iteration(3);
  
  // Verify the component was updated correctly
  test_assert(early_exit_updates == 2, 
              "Early exit component should have 2 updates, got %d", 
              early_exit_updates);
  
  test_assert(earlyExitComponent.state.value == 15, 
              "Early exit component value should be 15, got %d", 
              earlyExitComponent.state.value);
  
  // Verify the exit code
  test_assert(early_exit_code == 100, 
              "Exit code should be 100, got %d", 
              early_exit_code);
  
  return OK;
}
