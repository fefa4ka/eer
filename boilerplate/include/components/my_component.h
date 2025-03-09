/**
 * MyComponent Definition
 * 
 * This file defines a simple component with props and state.
 */

#pragma once

#include <eer.h>
#include <eer_comp.h>
#include <stdbool.h>
#include <stdio.h>

/* Define component props and state structures */
typedef struct {
    int value;
    // Add your props here
} MyComponent_props_t;

typedef struct {
    int value;
    int update_count;
    // Add your state here
} MyComponent_state_t;

/* Declare the component header */
eer_header(MyComponent);

/* Lifecycle method declarations */
void MyComponent_will_mount(void *instance, void *next_props);
bool MyComponent_should_update(void *instance, void *next_props);
void MyComponent_will_update(void *instance, void *next_props);
void MyComponent_release(void *instance);
void MyComponent_did_update(void *instance);
void MyComponent_did_mount(void *instance);
void MyComponent_did_unmount(void *instance);
