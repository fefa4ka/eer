#include <eer.h>

enum eer_context eer_staging(eer_t *instance, void *next_props)
{
    uintptr_t context = (uintptr_t)next_props;

    if(EER_CONTEXT_SAME == context) {
        if(EER_STAGE_RELEASED >= instance->stage.state.step)
            return EER_CONTEXT_SAME;
    } else if(EER_CONTEXT_UPDATED == context) {
        next_props = 0;
    } else if (EER_CONTEXT_BLOCKED == context) {
        instance->stage.state.step = EER_STAGE_UNMOUNTED;
    }

    if (EER_STAGE_RELEASED == instance->stage.state.step) {
        /* Check if priority 0, decrease priority on each step */
        if(!instance->should_update(instance, next_props)) {
            return EER_CONTEXT_SAME;
        }

        instance->stage.state.step = EER_STAGE_PREPARED;
        instance->will_update(instance, next_props);
    } else if (EER_STAGE_REACTING == instance->stage.state.step) {
        instance->stage.state.step = EER_STAGE_PREPARED;
        instance->will_update(instance, next_props);
        instance->stage.state.step = EER_STAGE_RELEASED;
        instance->release(instance);
        instance->did_update(instance);
    } else if (EER_STAGE_PREPARED == instance->stage.state.step) {
        instance->stage.state.step = EER_STAGE_RELEASED;
        instance->release(instance);
        instance->did_update(instance);
    } else if (EER_STAGE_DEFINED == instance->stage.state.step) {
#ifdef PROFILING
        hash_write(&eer_scope, eer_hash_component(instance->name), (void **)instance);
#endif
        instance->will_mount(instance, next_props);
        instance->release(instance);
        instance->did_mount(instance);
        instance->stage.state.step = EER_STAGE_RELEASED;
    } else if (EER_STAGE_UNMOUNTED == instance->stage.state.step) {
        instance->stage.state.step = EER_STAGE_BLOCKED;
        instance->did_unmount(instance);
    } else if (EER_STAGE_BLOCKED) {
        return EER_CONTEXT_BLOCKED;
    }

    return EER_CONTEXT_UPDATED;
}
