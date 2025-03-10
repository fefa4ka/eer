/**
 * Context Management Test
 *
 * This test verifies the behavior of the use and with macros
 * for managing component contexts.
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
  bool active;
} ContextComponent_props_t;

typedef struct {
  int value;
  int update_count;
  bool active;
} ContextComponent_state_t;

eer_header(ContextComponent);

WILL_MOUNT(ContextComponent) {
  state->value = props->value;
  state->update_count = 0;
  state->active = props->active;
  log_info("ContextComponent mounted with value: %d, active: %d", 
           state->value, state->active);
}

SHOULD_UPDATE(ContextComponent) {
  bool should_update = props->value != next_props->value || 
                       props->active != next_props->active;
  log_info("SHOULD_UPDATE called for component %p, returning %s", 
           self, should_update ? "true" : "false");
  return should_update;
}

WILL_UPDATE(ContextComponent) {
  log_info("WILL_UPDATE called for component %p", self);
}

RELEASE(ContextComponent) {
  state->value = props->value;
  state->active = props->active;
  state->update_count++;
  log_info("RELEASE called for component %p, value: %d, active: %d (update #%d)", 
           self, state->value, state->active, state->update_count);
}

DID_UPDATE(ContextComponent) {
  log_info("DID_UPDATE called for component %p", self);
}

DID_MOUNT(ContextComponent) {
  log_info("DID_MOUNT called for component %p", self);
}

DID_UNMOUNT(ContextComponent) {
  log_info("DID_UNMOUNT called for component %p", self);
}

/* Create component instances */
eer_withprops(ContextComponent, alwaysComponent, _({.value = 10, .active = true}));
eer_withprops(ContextComponent, conditionalComponent, _({.value = 20, .active = false}));
eer_withprops(ContextComponent, withComponent, _({.value = 30, .active = true}));

/* Global variables to store test results */
int always_updates = 0;
int conditional_updates = 0;
int with_updates = 0;

/* Hook functions to capture component state */
void after_context_update(void *data) {
  int *total_updates = (int *)data;
  always_updates = alwaysComponent.state.update_count;
  conditional_updates = conditionalComponent.state.update_count;
  with_updates = withComponent.state.update_count;
  *total_updates = always_updates + conditional_updates + with_updates;
  
  log_info("Component updates - always: %d, conditional: %d, with: %d, total: %d", 
           always_updates, conditional_updates, with_updates, *total_updates);
}

/* Test context management with use and with */
test(test_context_management) {
  int total_updates = 0;
  
  // Register hook to capture update counts
  test_hook_after_iteration(4, after_context_update, &total_updates);
  
  // Start loop with the always active component
  loop(alwaysComponent) {
    log_info("--- Iteration %d ---", eer_current_iteration);
    
    // Update the always active component
    apply(ContextComponent, alwaysComponent, 
          _({.value = alwaysComponent.state.value + 5, .active = true}));
    
    // Conditionally use the conditional component
    if (eer_current_iteration % 2 == 0) {
      log_info("Using conditional component (even iteration)");
      use(conditionalComponent);
      apply(ContextComponent, conditionalComponent, 
            _({.value = conditionalComponent.state.value + 10, .active = true}));
    } else {
      log_info("Skipping conditional component (odd iteration)");
    }
    
    // Use the with macro to create a new context
    with(withComponent) {
      log_info("Inside with context");
      apply(ContextComponent, withComponent, 
            _({.value = withComponent.state.value + 15, .active = true}));
    }
    
    // Exit after 4 iterations
    if (eer_current_iteration >= 4) {
      log_info("Test complete, exiting loop");
      eer_land.state.unmounted = true;
    }
  }
}

/* Verification function */
result_t test_context_management() {
  // Wait for test to complete
  test_wait_for_iteration(5);
  
  // Verify the always component was updated in every iteration
  test_assert(always_updates == 4, 
              "Always component should have 4 updates, got %d", 
              always_updates);
  
  // Verify the conditional component was only updated in even iterations
  test_assert(conditional_updates == 2, 
              "Conditional component should have 2 updates, got %d", 
              conditional_updates);
  
  // Verify the with component was updated in every iteration
  test_assert(with_updates == 4, 
              "With component should have 4 updates, got %d", 
              with_updates);
  
  // Verify the final values
  test_assert(alwaysComponent.state.value == 30, 
              "Always component value should be 30, got %d", 
              alwaysComponent.state.value);
  
  test_assert(conditionalComponent.state.value == 40, 
              "Conditional component value should be 40, got %d", 
              conditionalComponent.state.value);
  
  test_assert(withComponent.state.value == 90, 
              "With component value should be 90, got %d", 
              withComponent.state.value);
  
  return OK;
}
