#include <eer.h>

bool eer_staging(eer_t *instance, void *next_props)
{
    if (instance->stage == STAGE_RELEASED
        && instance->should_update(instance, next_props)) {
        instance->stage = STAGE_PREPARED;
        instance->will_update(instance, next_props);
    } else if (instance->stage == STAGE_PREPARED) {
        instance->stage = STAGE_RELEASED;
        instance->release(instance);
        instance->did_update(instance);
    } else if (instance->stage == STAGE_DEFINED) {
#ifdef PROFILING
        hash_write(&eer_scope, eer_hash_component(instance->name), (void **)instance);
#endif
        instance->will_mount(instance, next_props);
        instance->release(instance);
        instance->did_mount(instance);
        instance->stage = STAGE_RELEASED;
    } else if (instance->stage == STAGE_UNMOUNTED) {
        instance->stage = STAGE_BLOCKED;
        instance->did_unmount(instance);
    }

    return true;
}

bool eer_reacting(eer_t *instance, void *next_props)
{
    if (instance->should_update(instance, next_props)) {
        instance->stage = STAGE_PREPARED;
        instance->will_update(instance, next_props);
        instance->stage = STAGE_RELEASED;
        instance->release(instance);
        instance->did_update(instance);
    }

    if (instance->stage == STAGE_UNMOUNTED) {
        instance->stage = STAGE_BLOCKED;
        instance->did_unmount(instance);
    }

    return true;
}
