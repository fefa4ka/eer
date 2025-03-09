#include <eer.h>

/**
 * @brief Core function that manages component lifecycle transitions
 * 
 * This function is the heart of the EER framework's reactivity system.
 * It handles all transitions between component lifecycle states:
 * - DEFINED -> RELEASED (mounting)
 * - RELEASED -> PREPARED -> RELEASED (updating)
 * - REACTING -> PREPARED -> RELEASED (forced update)
 * - UNMOUNTED -> BLOCKED (unmounting)
 * 
 * @param instance Pointer to the component instance
 * @param next_props Either new props or a context flag
 * @return enum eer_context The new context state
 */
enum eer_context eer_staging(eer_t *instance, void *next_props)
{
    // Determine if next_props is actually a context flag
    uintptr_t context = (uintptr_t)next_props;

    // Handle different context flags
    if(EER_CONTEXT_SAME == context) {
        // No change needed if component is already mounted
        if(EER_STAGE_RELEASED >= instance->stage.state.step)
            return EER_CONTEXT_SAME;
    } else if(EER_CONTEXT_UPDATED == context) {
        // Context update without props change
        next_props = 0;
    } else if (EER_CONTEXT_BLOCKED == context) {
        // Block the component (prepare for unmounting)
        instance->stage.state.step = EER_STAGE_UNMOUNTED;
    }

    // Handle component lifecycle transitions based on current stage
    if (EER_STAGE_RELEASED == instance->stage.state.step) {
        /* Normal update path: Check if component should update */
        if(!instance->should_update(instance, next_props)) {
            return EER_CONTEXT_SAME; // Skip update if should_update returns false
        }

        // Prepare for update
        instance->stage.state.step = EER_STAGE_PREPARED;
        instance->will_update(instance, next_props);
    } else if (EER_STAGE_REACTING == instance->stage.state.step) {
        // Forced update path (react)
        instance->stage.state.step = EER_STAGE_PREPARED;
        instance->will_update(instance, next_props);
        instance->stage.state.step = EER_STAGE_RELEASED;
        instance->release(instance);
        instance->did_update(instance);
    } else if (EER_STAGE_PREPARED == instance->stage.state.step) {
        // Complete the update process
        instance->stage.state.step = EER_STAGE_RELEASED;
        instance->release(instance);
        instance->did_update(instance);
    } else if (EER_STAGE_DEFINED == instance->stage.state.step) {
        // Initial mounting process
#ifdef PROFILING
        hash_write(&eer_scope, eer_hash_component(instance->name), (void **)instance);
#endif
        // Execute mounting lifecycle
        instance->will_mount(instance, next_props);
        instance->release(instance);
        instance->did_mount(instance);
        instance->stage.state.step = EER_STAGE_RELEASED;
    } else if (EER_STAGE_UNMOUNTED == instance->stage.state.step) {
        // Unmounting process
        instance->stage.state.step = EER_STAGE_BLOCKED;
        if (instance->did_unmount) {
            instance->did_unmount(instance);
        }
    } else if (EER_STAGE_BLOCKED) {
        // Component is blocked, cannot process further
        return EER_CONTEXT_BLOCKED;
    }

    return EER_CONTEXT_UPDATED;
}
