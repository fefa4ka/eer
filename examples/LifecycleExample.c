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

// Define a component that uses hook-based lifecycle methods
typedef struct {
  int value;
} HookComponent_props_t;

typedef struct {
  int value;
} HookComponent_state_t;

eer_header(HookComponent);

// Use hook-based lifecycle methods
WILL_MOUNT_HOOK(HookComponent);
SHOULD_UPDATE_HOOK(HookComponent);
WILL_UPDATE_HOOK(HookComponent);
RELEASE_HOOK(HookComponent);
DID_MOUNT_HOOK(HookComponent);
DID_UPDATE_HOOK(HookComponent);
DID_UNMOUNT_HOOK(HookComponent);

// Callback functions
void hook_will_mount(HookComponent_t *self, HookComponent_props_t *next_props) {
  printf("Hook: WILL_MOUNT called\n");
  self->state.value = self->props.value;
}

bool hook_should_update(HookComponent_t *self, HookComponent_props_t *next_props) {
  printf("Hook: SHOULD_UPDATE called\n");
  return self->props.value != next_props->value;
}

void hook_will_update(HookComponent_t *self, HookComponent_props_t *next_props) {
  printf("Hook: WILL_UPDATE called\n");
}

void hook_release(HookComponent_t *self) {
  printf("Hook: RELEASE called\n");
  self->state.value = self->props.value;
}

void hook_did_mount(HookComponent_t *self) {
  printf("Hook: DID_MOUNT called\n");
}

void hook_did_update(HookComponent_t *self) {
  printf("Hook: DID_UPDATE called\n");
}

void hook_did_unmount(HookComponent_t *self) {
  printf("Hook: DID_UNMOUNT called\n");
}

// Create a hook-based component with callbacks
eer_withcallbacks(HookComponent, hookComponent, _({.value = 10}),
  .on_will_mount = hook_will_mount,
  .on_should_update = hook_should_update,
  .on_will_update = hook_will_update,
  .on_release = hook_release,
  .on_did_mount = hook_did_mount,
  .on_did_update = hook_did_update,
  .on_did_unmount = hook_did_unmount
);

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
  loop(lifecycleComponent, hookComponent, minimalComponent) {
    printf("\n--- Iteration ---\n");
    
    // Update the standard component
    apply(LifecycleComponent, lifecycleComponent,
          _({.value = lifecycleComponent.state.value + 1, 
             .force_update = false}));
    
    // Update the hook-based component
    apply(HookComponent, hookComponent,
          _({.value = hookComponent.state.value + 5}));
    
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
