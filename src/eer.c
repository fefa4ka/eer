#include <eer.h>

enum eer_context eer_staging(eer_t *instance, void *next_props)
{
    uintptr_t context = (uintptr_t)next_props;

    if(CONTEXT_SAME == context) {
        if(STAGE_RELEASED >= instance->stage.state.step)
            return CONTEXT_SAME;
    } else if(CONTEXT_UPDATED == context) {
        next_props = 0;
    } else if (CONTEXT_BLOCKED == context) {
        instance->stage.state.step = STAGE_UNMOUNTED;
    }

    if (STAGE_RELEASED == instance->stage.state.step) {
        /* Check if priority 0, decrease priority on each step */
        if(!instance->should_update(instance, next_props)) {
            return CONTEXT_SAME;
        }

        instance->stage.state.step = STAGE_PREPARED;
        instance->will_update(instance, next_props);
    } else if (STAGE_REACTING == instance->stage.state.step) {
        instance->stage.state.step = STAGE_PREPARED;
        instance->will_update(instance, next_props);
        instance->stage.state.step = STAGE_RELEASED;
        instance->release(instance);
        instance->did_update(instance);
    } else if (STAGE_PREPARED == instance->stage.state.step) {
        instance->stage.state.step = STAGE_RELEASED;
        instance->release(instance);
        instance->did_update(instance);
    } else if (STAGE_DEFINED == instance->stage.state.step) {
#ifdef PROFILING
        hash_write(&eer_scope, eer_hash_component(instance->name), (void **)instance);
#endif
        instance->will_mount(instance, next_props);
        instance->release(instance);
        instance->did_mount(instance);
        instance->stage.state.step = STAGE_RELEASED;
    } else if (STAGE_UNMOUNTED == instance->stage.state.step) {
        instance->stage.state.step = STAGE_BLOCKED;
        instance->did_unmount(instance);
    } else if (STAGE_BLOCKED) {
        return CONTEXT_BLOCKED;
    }

    return CONTEXT_UPDATED;
}
