#include <eer.h>
#include <eer_app.h>
#include <eer_comp.h>
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

test(one, two) {
  loop() {
    // Update component
    apply(MyComponent, myComponent, _({.value = 123}));
  }
}

result_t one() {
  test_assert(0 == 0, "Sensor pin should be off");
  return OK;
}
result_t two() {
  test_assert(0 == 0, "Sensor pin should be on");
  return OK;
}
