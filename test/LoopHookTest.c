#include "test_utils.h"
#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
#include "test.h"
#include <stdio.h>

// Define a simple component for testing
typedef struct {
  int value;
} HookTestComponent_props_t;

typedef struct {
  int value;
  int update_count;
} HookTestComponent_state_t;

eer_header(HookTestComponent);

WILL_MOUNT(HookTestComponent) {
  state->value = props->value;
  state->update_count = 0;
}

RELEASE(HookTestComponent) {
  state->value = props->value;
  state->update_count++;
}

// Skip methods we don't need for this test
SHOULD_UPDATE_SKIP(HookTestComponent);
WILL_UPDATE_SKIP(HookTestComponent);
DID_MOUNT_SKIP(HookTestComponent);
DID_UPDATE_SKIP(HookTestComponent);
DID_UNMOUNT_SKIP(HookTestComponent);

// Create component instance
eer_withprops(HookTestComponent, hookTestComponent, _({.value = 1}));

// Hook function to be called before the loop starts
void before_loop_hook(void *data) {
  int *value_ptr = (int *)data;
  *value_ptr = hookTestComponent.state.value;
  log_info("Before loop hook called, value = %d", *value_ptr);
}

// Hook function to be called after iteration 1
void after_iteration_1_hook(void *data) {
  int *value_ptr = (int *)data;
  *value_ptr = hookTestComponent.state.value;
  log_info("After iteration 1 hook called, value = %d", *value_ptr);
}

// Hook function to be called after iteration 2
void after_iteration_2_hook(void *data) {
  int *value_ptr = (int *)data;
  *value_ptr = hookTestComponent.state.value;
  log_info("After iteration 2 hook called, value = %d", *value_ptr);
}

// Hook function to be called after iteration 3
void after_iteration_3_hook(void *data) {
  int *value_ptr = (int *)data;
  *value_ptr = hookTestComponent.state.value;
  log_info("After iteration 3 hook called, value = %d", *value_ptr);
}

// Hook function to be called after iteration 4
void after_iteration_4_hook(void *data) {
  int *value_ptr = (int *)data;
  *value_ptr = hookTestComponent.state.value;
  log_info("After iteration 4 hook called, value = %d", *value_ptr);
}
         
// Hook function to be called before exit
void before_exit_hook(void *data) {
  int *value_ptr = (int *)data;
  *value_ptr = hookTestComponent.state.value;
  log_info("Before exit hook called, value = %d", *value_ptr);
}

  int before_loop_value = 0;
  int iteration_1_value = 0;
  int iteration_2_value = 0;
  int iteration_3_value = 0;
  int iteration_4_value = 0;
  int before_exit_value = 0;

// Test function to verify hooks work correctly
result_t test_loop_hooks() {
  // Wait for all hooks to execute
  test_wait_for_iteration(6);

  // Verify before loop hook
  test_assert(before_loop_value == 0,
              "Before loop hook should capture initial value 0, got %d",
              before_loop_value);

  // Verify after iteration 2 hook
  test_assert(iteration_2_value == 3,
              "After iteration 2 hook should capture value 3, got %d",
              iteration_2_value);

  // Wait a bit for the before_exit_hook to execute
  usleep(10000); // 10ms should be enough

  // Verify before exit hook
  test_assert(before_exit_value == 7,
              "Before exit hook should capture final value 7, got %d",
              before_exit_value);

  return OK;
}

// Main test function
test(test_loop_hooks) {

  // Register hooks
  test_hook_before_loop(before_loop_hook, &before_loop_value);
  test_hook_after_iteration(1, after_iteration_1_hook, &iteration_1_value);
  test_hook_after_iteration(2, after_iteration_2_hook, &iteration_2_value);
  test_hook_after_iteration(3, after_iteration_3_hook, &iteration_3_value);
  test_hook_after_iteration(4, after_iteration_4_hook, &iteration_4_value);
  test_hook_before_exit(before_exit_hook, &before_exit_value);

  // Start the event loop with our component
  loop(hookTestComponent) {
    // Update the component value on each iteration
    apply(HookTestComponent, hookTestComponent,
          _({.value = hookTestComponent.state.value + 1}));

    // Exit after 5 iterations
    if (eer_current_iteration >= 5) {
        // Apply one final update before exiting
        apply(HookTestComponent, hookTestComponent,
              _({.value = hookTestComponent.state.value + 1}));
        
        eer_land.state.unmounted = true;
        log_info("Unmounted");
    }
  }
}
