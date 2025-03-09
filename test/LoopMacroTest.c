/**
 * Loop Macro Test
 *
 * This test verifies the behavior of the loop macro with both
 * single and multiple components.
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

/* Define a second component for multi-component testing */
typedef struct {
  int value;
} SecondComponent_props_t;

typedef struct {
  int value;
  int update_count;
} SecondComponent_state_t;

eer_header(SecondComponent);

WILL_MOUNT(SecondComponent) {
  state->value = props->value;
  state->update_count = 0;
  log_info("SecondComponent mounted with value: %d", state->value);
}

SHOULD_UPDATE_SKIP(SecondComponent);
WILL_UPDATE_SKIP(SecondComponent);

RELEASE(SecondComponent) {
  state->value = props->value;
  state->update_count++;
  log_info("SecondComponent updated to value: %d (update #%d)", 
           state->value, state->update_count);
}

DID_UPDATE_SKIP(SecondComponent);
DID_MOUNT_SKIP(SecondComponent);
DID_UNMOUNT_SKIP(SecondComponent);

/* Create component instances */
eer_withprops(SimpleLoopComponent, singleComponent, _({.value = 10}));
eer_withprops(SimpleLoopComponent, firstComponent, _({.value = 100}));
eer_withprops(SecondComponent, secondComponent, _({.value = 200}));

/* Global variables to store test results */
int single_component_updates = 0;
int first_component_updates = 0;
int second_component_updates = 0;

/* Hook functions to capture component state */
void after_single_update(void *data) {
  int *updates = (int *)data;
  *updates = singleComponent.state.update_count;
  log_info("Single component updates: %d", *updates);
}

void after_multi_update(void *data) {
  int *updates = (int *)data;
  first_component_updates = firstComponent.state.update_count;
  second_component_updates = secondComponent.state.update_count;
  *updates = first_component_updates + second_component_updates;
  log_info("First component updates: %d, Second component updates: %d", 
           first_component_updates, second_component_updates);
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

/* Test multiple component loop */
test(test_multiple_component_loop) {
  int total_updates = 0;
  
  // Register hook to capture update counts
  test_hook_after_iteration(3, after_multi_update, &total_updates);
  
  // Start loop with multiple components
  loop(firstComponent, secondComponent) {
    // Update both components
    apply(SimpleLoopComponent, firstComponent, 
          _({.value = firstComponent.state.value + 10}));
    apply(SecondComponent, secondComponent, 
          _({.value = secondComponent.state.value + 20}));
    
    // Exit after 3 iterations
    if (eer_current_iteration >= 3) {
      eer_land.state.unmounted = true;
    }
  }
}

/* Verification functions */
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

result_t test_multiple_component_loop() {
  // Wait for test to complete
  test_wait_for_iteration(4);
  
  // Verify both components were updated correctly
  test_assert(first_component_updates == 3, 
              "First component should have 3 updates, got %d", 
              first_component_updates);
  
  test_assert(second_component_updates == 3, 
              "Second component should have 3 updates, got %d", 
              second_component_updates);
  
  test_assert(firstComponent.state.value == 130, 
              "First component value should be 130, got %d", 
              firstComponent.state.value);
  
  test_assert(secondComponent.state.value == 260, 
              "Second component value should be 260, got %d", 
              secondComponent.state.value);
  
  return OK;
}
