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

