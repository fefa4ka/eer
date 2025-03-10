#pragma once

#include <stdbool.h>
#include <stdint.h>

// Loop event types
typedef enum {
    EER_LOOP_BEFORE_START,    // Before the loop starts
    EER_LOOP_AFTER_ITERATION, // After a specific iteration
    EER_LOOP_BEFORE_EXIT      // Before the loop exits
} eer_loop_event_t;

// Loop hook function type
typedef void (*eer_loop_hook_fn)(void* data);

// Loop hook structure
typedef struct {
    eer_loop_event_t event;
    uint64_t iteration;            // For AFTER_ITERATION events
    eer_loop_hook_fn hook;
    void* data;
    bool executed;
} eer_loop_hook_t;

// Maximum number of hooks that can be registered
#define EER_MAX_HOOKS 10

// Global hook registry
extern eer_loop_hook_t eer_hooks[EER_MAX_HOOKS];
extern int eer_hook_count;
extern uint64_t eer_current_iteration;

// Initialize the hook system
void eer_hooks_init(void);

// Register a hook to be called before the loop starts
void eer_hook_before_loop(eer_loop_hook_fn hook, void* data);

// Register a hook to be called after a specific iteration
void eer_hook_after_iteration(int iteration, eer_loop_hook_fn hook, void* data);

// Register a hook to be called before the loop exits
void eer_hook_before_exit(eer_loop_hook_fn hook, void* data);

// Execute hooks for a specific event
void eer_execute_hooks(eer_loop_event_t event);

// Execute hooks for a specific event with a delay
void eer_execute_hooks_delayed(eer_loop_event_t event, int delay_ms);

// Wait for a specific iteration to complete
void eer_wait_for_iteration(int iteration);

// Increment the iteration counter and execute hooks
void eer_increment_iteration(void);

// Reset the hook system
void eer_hooks_reset(void);
