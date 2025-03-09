#include "test_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Global hook registry
eer_loop_hook_t eer_hooks[EER_MAX_HOOKS];
int eer_hook_count = 0;
int eer_current_iteration = 0;

// Initialize the hook system
void eer_hooks_init(void) {
    eer_hook_count = 0;
    eer_current_iteration = 0;
    for (int i = 0; i < EER_MAX_HOOKS; i++) {
        eer_hooks[i].executed = false;
    }
}

// Register a hook to be called before the loop starts
void eer_hook_before_loop(eer_loop_hook_fn hook, void* data) {
    if (eer_hook_count >= EER_MAX_HOOKS) {
        fprintf(stderr, "Error: Too many hooks registered\n");
        return;
    }
    
    eer_hooks[eer_hook_count].event = EER_LOOP_BEFORE_START;
    eer_hooks[eer_hook_count].hook = hook;
    eer_hooks[eer_hook_count].data = data;
    eer_hooks[eer_hook_count].executed = false;
    eer_hook_count++;
}

// Register a hook to be called after a specific iteration
void eer_hook_after_iteration(int iteration, eer_loop_hook_fn hook, void* data) {
    if (eer_hook_count >= EER_MAX_HOOKS) {
        fprintf(stderr, "Error: Too many hooks registered\n");
        return;
    }
    
    eer_hooks[eer_hook_count].event = EER_LOOP_AFTER_ITERATION;
    eer_hooks[eer_hook_count].iteration = iteration;
    eer_hooks[eer_hook_count].hook = hook;
    eer_hooks[eer_hook_count].data = data;
    eer_hooks[eer_hook_count].executed = false;
    eer_hook_count++;
}

// Register a hook to be called before the loop exits
void eer_hook_before_exit(eer_loop_hook_fn hook, void* data) {
    if (eer_hook_count >= EER_MAX_HOOKS) {
        fprintf(stderr, "Error: Too many hooks registered\n");
        return;
    }
    
    eer_hooks[eer_hook_count].event = EER_LOOP_BEFORE_EXIT;
    eer_hooks[eer_hook_count].hook = hook;
    eer_hooks[eer_hook_count].data = data;
    eer_hooks[eer_hook_count].executed = false;
    eer_hook_count++;
}

// Execute hooks for a specific event
void eer_execute_hooks(eer_loop_event_t event) {
    for (int i = 0; i < eer_hook_count; i++) {
        if (eer_hooks[i].event == event) {
            // For iteration events, check if this is the right iteration
            if (event == EER_LOOP_AFTER_ITERATION && 
                eer_hooks[i].iteration != eer_current_iteration) {
                continue;
            }
            
            // Only execute hooks once
            if (!eer_hooks[i].executed) {
                eer_hooks[i].hook(eer_hooks[i].data);
                eer_hooks[i].executed = true;
            }
        }
    }
}

// Wait for a specific iteration to complete
void eer_wait_for_iteration(int iteration) {
    while (eer_current_iteration < iteration) {
        usleep(1000); // Sleep for 1ms to avoid busy waiting
    }
}

// Increment the iteration counter and execute hooks
void eer_increment_iteration(void) {
    eer_current_iteration++;
    eer_execute_hooks(EER_LOOP_AFTER_ITERATION);
}

// Reset the hook system
void eer_hooks_reset(void) {
    eer_hook_count = 0;
    eer_current_iteration = 0;
}
