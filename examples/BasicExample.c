/**
 * Basic Example
 *
 * This is the simplest possible example using the EER Framework.
 * It demonstrates the basic component lifecycle and event loop.
 */

#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
#include <stdio.h>
#include <unistd.h>

// Define a basic component
typedef struct {
  int value;
} BasicComponent_props_t;

typedef struct {
  int value;
  int update_count;
} BasicComponent_state_t;

eer_header(BasicComponent);

// Lifecycle methods
WILL_MOUNT(BasicComponent) {
  state->value = props->value;
  state->update_count = 0;
  printf("BasicComponent initialized with value: %d\n", state->value);
}

SHOULD_UPDATE_SKIP(BasicComponent);
WILL_UPDATE_SKIP(BasicComponent);

RELEASE(BasicComponent) {
  state->value = props->value;
  state->update_count++;
}

DID_UPDATE(BasicComponent) {
  printf("BasicComponent updated to value: %d (update count: %d)\n",
         state->value, state->update_count);
}

DID_MOUNT(BasicComponent) { printf("BasicComponent mounted\n"); }

DID_UNMOUNT(BasicComponent) { printf("BasicComponent unmounted\n"); }

// Create a component instance with initial props
eer_withprops(BasicComponent, basicComponent, _({.value = 1}));

int main() {
  printf("Starting Basic Example\n");

  // Start the event loop
  loop(basicComponent) {
    // Update the component value
    apply(BasicComponent, basicComponent,
          _({.value = basicComponent.state.value + 1}));

    // Exit after 5 updates
    if (basicComponent.state.update_count >= 5) {
      printf("Basic Example send unmount signal\n");
      eer_land.state.unmounted = true;
    }
  }

  printf("Basic Example Completed\n");
  return 0;
}
