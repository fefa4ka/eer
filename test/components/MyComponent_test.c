#include <criterion/criterion.h>
#include <eer.h>
#include <eers_app.h>
#include <stdio.h>

// Component declaration from README example
eer_header(MyComponent);

typedef struct {
  int value;
} MyComponent_props_t;

typedef struct {
  int value;
  bool initialized;
  bool updated;
} MyComponent_state_t;

WILL_MOUNT(MyComponent) {
  state->initialized = true;
  state->value = props->value;
}

WILL_UPDATE(MyComponent) {
  state->updated = false;
}

RELEASE(MyComponent) {
  state->value = props->value;
}

DID_UPDATE(MyComponent) {
  state->updated = true;
}

DID_MOUNT(MyComponent) {
  // Mount verification done in test
}

void MyComponent_reset(eer_t *instance) {
  MyComponent_state_t *state = eer_state(MyComponent, instance);
  state->value = 0;
}

int MyComponent_getValue(eer_t *instance) {
  MyComponent_state_t *state = eer_state(MyComponent, instance);
  return state->value;
}

Test(MyComponent, lifecycle) {
  MyComponent(myComponent, _({.value = 42}));
  
  ignite(myComponent);
  loop(myComponent) {
    // Verify initial state
    cr_assert_eq(eer_state(MyComponent, myComponent, initialized), true);
    cr_assert_eq(eer_state(MyComponent, myComponent, value), 42);
    cr_assert_eq(eer_state(MyComponent, myComponent, updated), false);

    // Update component
    apply(MyComponent, myComponent, _({.value = 123}));
    
    // Verify update
    cr_assert_eq(eer_state(MyComponent, myComponent, value), 123);
    cr_assert_eq(eer_state(MyComponent, myComponent, updated), true);

    // Test reset function
    MyComponent_reset(&myComponent.instance);
    cr_assert_eq(MyComponent_getValue(&myComponent.instance), 0);
    
    terminate;
  }
}
