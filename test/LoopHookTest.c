#include "test.h"
#include "test_utils.h"
#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
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

// Hook function to be called after iteration 2
void after_iteration_2_hook(void *data) {
  int *value_ptr = (int *)data;
  *value_ptr = hookTestComponent.state.value;
  log_info("After iteration 2 hook called, value = %d", *value_ptr);
}

// Hook function to be called before exit
void before_exit_hook(void *data) {
  int *value_ptr = (int *)data;
  *value_ptr = hookTestComponent.state.value;
  log_info("Before exit hook called, value = %d", *value_ptr);
}

// Test function to verify hooks work correctly
result_t test_loop_hooks() {
  int before_loop_value = 0;
  int iteration_2_value = 0;
  int before_exit_value = 0;

  // Wait for all hooks to execute
  test_wait_for_iteration(5);

  // Verify before loop hook
  test_assert(before_loop_value == 1,
              "Before loop hook should capture initial value 1, got %d",
              before_loop_value);

  // Verify after iteration 2 hook
  test_assert(iteration_2_value == 3,
              "After iteration 2 hook should capture value 3, got %d",
              iteration_2_value);

  // Verify before exit hook
  test_assert(before_exit_value == 6,
              "Before exit hook should capture final value 6, got %d",
              before_exit_value);

  return OK;
}

// Main test function
test(test_loop_hooks) {
  int before_loop_value = 0;
  int iteration_2_value = 0;
  int before_exit_value = 0;

  // Register hooks
  test_hook_before_loop(before_loop_hook, &before_loop_value);
  test_hook_after_iteration(2, after_iteration_2_hook, &iteration_2_value);
  test_hook_before_exit(before_exit_hook, &before_exit_value);

  // Start the event loop
  loop() {
    // Update the component value on each iteration
    apply(HookTestComponent, hookTestComponent,
          _({.value = hookTestComponent.state.value + 1}));

    // Exit after 5 iterations
    if (eer_current_iteration >= 5) {
      break;
    }
  }
}
