/**
 * Lifecycle Example
 *
 * This example demonstrates the complete component lifecycle
 * and different ways to implement lifecycle hooks.
 */

#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

// Define a component with full lifecycle implementation
typedef struct {
  int value;
  bool force_update;
} LifecycleComponent_props_t;

typedef struct {
  int value;
  int update_count;
  int mount_time;
  int last_update_time;
} LifecycleComponent_state_t;

eer_header(LifecycleComponent);

// Full lifecycle implementation
WILL_MOUNT(LifecycleComponent) {
  printf("WILL_MOUNT: Initializing component\n");
  state->value = props->value;
  state->update_count = 0;
  state->mount_time = time(NULL);
}

SHOULD_UPDATE(LifecycleComponent) {
  printf("SHOULD_UPDATE: Checking if update is needed\n");
  // Either force update or only update when value changes
  return next_props->force_update || props->value != next_props->value;
}

WILL_UPDATE(LifecycleComponent) {
  printf("WILL_UPDATE: Preparing for update from %d to %d\n", 
         state->value, next_props->value);
}

RELEASE(LifecycleComponent) {
  printf("RELEASE: Updating state with new props\n");
  state->value = props->value;
  state->update_count++;
  state->last_update_time = time(NULL);
}

DID_UPDATE(LifecycleComponent) {
  printf("DID_UPDATE: Component updated to value %d (update #%d)\n", 
         state->value, state->update_count);
}

DID_MOUNT(LifecycleComponent) {
  printf("DID_MOUNT: Component mounted with value %d\n", state->value);
}

DID_UNMOUNT(LifecycleComponent) {
  printf("DID_UNMOUNT: Component unmounted after %d updates\n", 
         state->update_count);
}

// Create a component instance with initial props
eer_withprops(LifecycleComponent, lifecycleComponent, _({
  .value = 1,
  .force_update = false
}));

// Define a standard component to replace the hook-based one
typedef struct {
  int value;
} StandardComponent_props_t;

typedef struct {
  int value;
} StandardComponent_state_t;

eer_header(StandardComponent);

// Standard lifecycle implementation
WILL_MOUNT(StandardComponent) {
  printf("Standard: WILL_MOUNT called\n");
  state->value = props->value;
}

SHOULD_UPDATE(StandardComponent) {
  printf("Standard: SHOULD_UPDATE called\n");
  return props->value != next_props->value;
}

WILL_UPDATE(StandardComponent) {
  printf("Standard: WILL_UPDATE called\n");
}

RELEASE(StandardComponent) {
  printf("Standard: RELEASE called\n");
  state->value = props->value;
}

DID_MOUNT(StandardComponent) {
  printf("Standard: DID_MOUNT called\n");
}

DID_UPDATE(StandardComponent) {
  printf("Standard: DID_UPDATE called\n");
}

DID_UNMOUNT(StandardComponent) {
  printf("Standard: DID_UNMOUNT called\n");
}

// Create a standard component with initial props
eer_withprops(StandardComponent, standardComponent, _({.value = 10}));

// Define a component with minimal lifecycle implementation
typedef struct {
  int value;
} MinimalComponent_props_t;

typedef struct {
  int value;
} MinimalComponent_state_t;

eer_header(MinimalComponent);

// Only implement essential methods, skip the rest
WILL_MOUNT_SKIP(MinimalComponent);
SHOULD_UPDATE_SKIP(MinimalComponent);
WILL_UPDATE_SKIP(MinimalComponent);

RELEASE(MinimalComponent) {
  printf("Minimal: RELEASE called\n");
  state->value = props->value;
}

DID_MOUNT_SKIP(MinimalComponent);
DID_UPDATE_SKIP(MinimalComponent);
DID_UNMOUNT_SKIP(MinimalComponent);

// Create a minimal component
eer(MinimalComponent, minimalComponent);

int main() {
  printf("Starting Lifecycle Example\n");
  printf("=========================\n\n");

  // Start the event loop
  loop(lifecycleComponent, standardComponent, minimalComponent) {
    printf("\n--- Iteration ---\n");
    
    // Update the standard component
    apply(LifecycleComponent, lifecycleComponent,
          _({.value = lifecycleComponent.state.value + 1, 
             .force_update = false}));
    
    // Update the standard component
    apply(StandardComponent, standardComponent,
          _({.value = standardComponent.state.value + 5}));
    
    // Update the minimal component
    apply(MinimalComponent, minimalComponent,
          _({.value = minimalComponent.state.value + 1}));
    
    // Sleep to make output readable
    usleep(500000);
    
    // Exit after a few iterations
    if (lifecycleComponent.state.update_count >= 3) {
      printf("\nExiting after %d updates\n", 
             lifecycleComponent.state.update_count);
      eer_land.state.unmounted = true;
    }
  }

  printf("\nLifecycle Example Completed\n");
  return 0;
}
