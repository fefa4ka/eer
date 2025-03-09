/**
 * MyComponent Implementation
 * 
 * This file implements the lifecycle methods for MyComponent.
 */

#include "components/my_component.h"

/* Implement lifecycle methods */

// Called when component is first mounted
WILL_MOUNT(MyComponent) {
    // Initialize state based on props
    state->value = props->value;
    state->update_count = 0;
    printf("MyComponent initialized with value: %d\n", state->value);
}

// Determines if component should update
SHOULD_UPDATE(MyComponent) {
    // Only update if value has changed
    return props->value != next_props->value;
}

// Called before update
WILL_UPDATE(MyComponent) {
    printf("MyComponent preparing to update from %d to %d\n", 
           state->value, next_props->value);
}

// Main update function - apply props to state
RELEASE(MyComponent) {
    state->value = props->value;
    state->update_count++;
}

// Called after update
DID_UPDATE(MyComponent) {
    printf("MyComponent updated to value: %d (update count: %d)\n",
           state->value, state->update_count);
}

// Called after mount
DID_MOUNT(MyComponent) {
    printf("MyComponent mounted\n");
}

// Called when unmounted
DID_UNMOUNT(MyComponent) {
    printf("MyComponent unmounted\n");
}
