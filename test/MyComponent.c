#include <eer.h>
#include <eer_app.h>
#include "test.h"
#include <stdio.h>

#define MyComponent(instance) eer(MyComponent, instance)
#define MyComponent_new(instance, state)                                       \
  eer_withstate(MyComponent, instance, _(state))
typedef struct {
  int value;
} MyComponent_props_t;

typedef struct {
  int value;
  bool initialized;
  bool updated;
} MyComponent_state_t;

eer_header(MyComponent);

WILL_MOUNT(MyComponent) {
  state->initialized = true;
  state->value = props->value;
}

WILL_UPDATE(MyComponent) { state->updated = false; }

SHOULD_UPDATE(MyComponent) {
  return true; // Always update by default
}

RELEASE(MyComponent) { state->value = props->value; }

DID_UPDATE(MyComponent) { state->updated = true; }

DID_MOUNT(MyComponent) {
  // Mount verification done in test
}

MyComponent_new(myComponent, _({.value = 42}));

test(check_my_component) {
  loop() {
    // Update component
    apply(MyComponent, myComponent, _({.value = 123}));
  }
}

result_t check_my_component() {
  test_assert(myComponent.state.initialized == false,
              "Not Intitialized MyComponent");

  test_assert(myComponent.state.value == 42,
              "Not Intitialized MyComponent value=%d", myComponent.state.value);
  usleep(5);
  test_assert(myComponent.state.initialized == true,
              "Intitialized MyComponent");
  test_assert(myComponent.state.value == 123,
              "Intitialized MyComponent value=%d", myComponent.state.value);

  return OK;
}
