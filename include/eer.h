#pragma once

#include "magic.h"
#include <stdbool.h>
#include <stdint.h>

/*
 *        ┌───────┐  will_mount  ┌────────┐   should_update?  ┌────────┐
 *        │DEFINED├─────────────►│RELEASED├──────────────────►│PREPARED├──┐
 *        └───────┘  release     └────────┘   will_update     └────────┘  │
 *                   did_mount        ▲                        release    │
 *                                    ├───────────────────────────────────┘
 *                                    ▼                        did_update
 *                               ┌─────────┐
 *                               │UNMOUNTED│
 *                               └─────────┘
 */

#define eer_header(Type)                                                       \
    typedef struct Type {                                                      \
        eer_t          instance;                                               \
        Type##_props_t props;                                                  \
        Type##_state_t state;                                                  \
    } Type##_t;                                                                \
    void Type##_will_mount(void *instance, void *next_props);                  \
    bool Type##_should_update(void *instance, void *next_props);               \
    void Type##_will_update(void *instance, void *next_props);                 \
    void Type##_release(void *instance);                                       \
    void Type##_did_mount(void *instance);                                     \
    void Type##_did_unmount(void *instance);                                   \
    void Type##_did_update(void *instance)

#define eer_define_component(Type, name)                                       \
    {                                                                          \
        .stage = {STAGE_DEFINED}, .will_mount = Type##_will_mount,             \
        .should_update = Type##_should_update,                                 \
        .will_update = Type##_will_update, .release = Type##_release,          \
        .did_mount = Type##_did_mount, .did_update = Type##_did_update,        \
    }

#define eer_self(Type, instance) Type##_t *self = instance

#define eer_prop(Type, instance, attribute)                                    \
    (((Type##_t *)instance)->props.attribute)
#define eer_state(Type, instance, attribute)                                   \
    (((Type##_t *)instance)->state.attribute)
#define eer_from_props(Type, props) (Type##_t *)(props - sizeof(eer_t))
#define eer_from_state(Type, props)                                            \
    eer_from_props(state - sizeof(Type##_props_t))

#define eer_instance_state(Type, instance)                                     \
    ((Type##_state_t *)((instance)->state))
#define eer_instance_props(Type, instance)                                     \
    ((Type##_props_t *)((instance)->props))

#if __GNUC__
    #define eer_lifecycle_header(Type, stage)                                  \
        static inline void Type##_inline_##stage(                              \
            eer_t *self, Type##_props_t *props, Type##_state_t *state)
#else
    #define eer_lifecycle_header(Type, stage)                                  \
        inline void Type##_inline_##stage(eer_t *self, Type##_props_t *props,  \
                                          Type##_state_t *state)
#endif

#define eer_lifecycle_prepare(Type, instance, stage) eer_hw_isr_disable()
#define eer_lifecycle_finish(Type, instance, stage)  eer_hw_isr_enable()

#define eer_lifecycle(Type, stage)                                             \
    eer_lifecycle_header(Type, stage);                                         \
    void Type##_##stage(void *instance)                                        \
    {                                                                          \
        eer_self(Type, instance);                                              \
        eer_lifecycle_prepare(Type, instance, stage);                          \
        Type##_inline_##stage(&self->instance, &self->props, &self->state);    \
        eer_lifecycle_finish(Type, instance, stage);                           \
    }                                                                          \
    eer_lifecycle_header(Type, stage)

#define eer_selfnext(Type, instance) Type##_props_t *next_props = next_props_ptr

#if __GNUC__
    #define eer_updatecycle_header(Type, stage, returnType)                    \
        static inline returnType Type##_inline_##stage(                        \
            eer_t *self, Type##_props_t *props, Type##_state_t *state,         \
            Type##_props_t *next_props)
#else
    #define eer_updatecycle_header(Type, stage, returnType)                    \
        inline returnType Type##_inline_##stage(                               \
            eer_t *self, Type##_props_t *props, Type##_state_t *state,         \
            Type##_props_t *next_props)
#endif

#define eer_updatecycle(Type, stage, returnType)                               \
    eer_updatecycle_header(Type, stage, returnType);                           \
    returnType Type##_##stage(void *instance, void *next_props_ptr)            \
    {                                                                          \
        eer_self(Type, instance);                                              \
        if (!next_props_ptr)                                                   \
            next_props_ptr = &self->props;                                     \
        eer_selfnext(Type, instance);                                          \
        eer_lifecycle_prepare(Type, instance, stage);                          \
        returnType result = Type##_inline_##stage(                             \
            &self->instance, &self->props, &self->state, next_props);          \
        eer_lifecycle_finish(Type, instance, stage);                           \
        return result;                                                         \
    }                                                                          \
    eer_updatecycle_header(Type, stage, returnType)

#define eer_will_mount(Type)                                                   \
    eer_updatecycle_header(Type, will_mount, void);                            \
    void Type##_will_mount(void *instance, void *next_props_ptr)               \
    {                                                                          \
        eer_self(Type, instance);                                              \
        if (!next_props_ptr)                                                   \
            next_props_ptr = &self->props;                                     \
        eer_selfnext(Type, instance);                                          \
        if (&self->props != next_props)                                        \
            self->props = *next_props;                                         \
        eer_lifecycle_prepare(Type, instance, will_mount);                     \
        Type##_inline_will_mount(&self->instance, &self->props, &self->state,  \
                                 next_props);                                  \
        eer_lifecycle_finish(Type, instance, will_mount);                      \
    }                                                                          \
    eer_updatecycle_header(Type, will_mount, void)

#define eer_will_update(Type)                                                  \
    eer_updatecycle_header(Type, will_update, void);                           \
    void Type##_will_update(void *instance, void *next_props_ptr)              \
    {                                                                          \
        eer_self(Type, instance);                                              \
        if (!next_props_ptr)                                                   \
            next_props_ptr = &self->props;                                     \
        eer_selfnext(Type, instance);                                          \
        eer_lifecycle_prepare(Type, instance, will_update);                    \
        Type##_inline_will_update(&self->instance, &self->props, &self->state, \
                                  next_props);                                 \
        eer_lifecycle_finish(Type, instance, will_update);                     \
        if (&self->props != next_props)                                        \
            self->props = *next_props;                                         \
    }                                                                          \
    eer_updatecycle_header(Type, will_update, void)

#define eer_release(Type)       eer_lifecycle(Type, release)
#define eer_should_update(Type) eer_updatecycle(Type, should_update, bool)
#define eer_did_mount(Type)     eer_lifecycle(Type, did_mount)
#define eer_did_update(Type)    eer_lifecycle(Type, did_update)
#define eer_did_unmount(Type)   eer_lifecycle(Type, did_unmount)

/* Skip */
#define eer_lifecycle_skip(Type, stage)                                        \
    void Type##_##stage(void *instance)                                        \
    {                                                                          \
    }

#define eer_updatecycle_skip(Type, stage, return_type)                         \
    return_type Type##_##stage(void *instance, void *next_props_ptr)           \
    {                                                                          \
    }


#define eer_will_mount_skip(Type)                                              \
    void Type##_will_mount(void *instance, void *next_props_ptr)               \
    {                                                                          \
        eer_self(Type, instance);                                              \
        if (next_props_ptr) {                                                  \
            eer_selfnext(Type, instance);                                      \
            if (&self->props != next_props)                                    \
                self->props = *next_props;                                     \
        }                                                                      \
    }
#define eer_will_update_skip(Type)                                             \
    void Type##_will_update(void *instance, void *next_props_ptr)              \
    {                                                                          \
        eer_self(Type, instance);                                              \
        if (next_props_ptr) {                                                  \
            eer_selfnext(Type, instance);                                      \
            if (&self->props != next_props)                                    \
                self->props = *next_props;                                     \
        }                                                                      \
    }

#define eer_release_skip(Type) eer_lifecycle_skip(Type, release)
#define eer_should_update_skip(Type)                                           \
    bool Type##_should_update(void *instance, void *next_props_ptr)            \
    {                                                                          \
        return true;                                                           \
    }
#define eer_did_mount_skip(Type)   eer_lifecycle_skip(Type, did_mount)
#define eer_did_update_skip(Type)  eer_lifecycle_skip(Type, did_update)
#define eer_did_unmount_skip(Type) eer_lifecycle_skip(Type, did_unmount)

/* Hooks */
#define eer_lifecycle_hook(Type, stage, callback)                              \
    void Type##_##stage(void *instance)                                        \
    {                                                                          \
        eer_self(Type, instance);                                              \
        if (self->callback)                                                    \
            self->callback(self);                                              \
    }

#define eer_release_hook(Type)     eer_lifecycle_hook(Type, release)
#define eer_did_mount_hook(Type)   eer_lifecycle_hook(Type, did_mount)
#define eer_did_update_hook(Type)  eer_lifecycle_hook(Type, did_update)
#define eer_did_unmount_hook(Type) eer_lifecycle_hook(Type, did_unmount)

#define eer_withprops(Type, instance_name, instance_props)                     \
    Type##_t instance_name = {                                                 \
        .instance = eer_define_component(Type, instance_name),                 \
        .props    = instance_props,                                            \
    }

#define eer_withstate(Type, instance_name, instance_state)                     \
    Type##_t instance_name = {                                                 \
        .instance = eer_define_component(Type, instance_name),                 \
        .state    = instance_state,                                            \
    }

#define eer(Type, name) eer_withprops(Type, name, _({0}))

#define eer_define(Type, instance_name, instance_props, instance_state)        \
    Type##_t instance_name = {                                                 \
        .instance = eer_define_component(Type, instance_name),                 \
        .props    = instance_props,                                            \
        .state    = instance_state,                                            \
    }

#define _(...) __VA_ARGS__

#define eer_apply(Type, name, propsValue)                                      \
    if (CONTEXT_UPDATED == eer_land.state.context                              \
        && (STAGE_RELEASED == name.instance.stage.state.step                   \
            || STAGE_DEFINED == name.instance.stage.state.step)) {             \
        eer_lifecycle_prepare(Type, &name, next_props);                        \
        Type##_props_t next_props = propsValue;                                \
        eer_lifecycle_finish(Type, &name, next_props);                         \
        eer_staging(&name.instance, &next_props);                              \
    } else {                                                                   \
        eer_staging(&name.instance, 0);                                        \
    }
/* Finalize previous step, and full cycle */
#define eer_react(Type, name, propsValue)                                      \
    {                                                                          \
        Type##_props_t next_props = propsValue;                                \
        eer_staging(&name.instance, eer_land.state.context);                   \
        if (CONTEXT_BLOCKED != eer_land.state.context) {                       \
            &name.instance.stage.state.step = STAGE_REACTING;                  \
            eer_staging(&name.instance, &next_props);                          \
        }                                                                      \
    }

#define eer_shut(x)                                                            \
    x.instance.stage = STAGE_UNMOUNTED;                                        \
    eer_staging(&x.instance, 0);

#define __eer_use(x)                                                           \
    eer_staging(&(x.instance), (void *)(uintptr_t)eer_land.state.context);
#define eer_use(...) EVAL(MAP(__eer_use, __VA_ARGS__))

#define __eer_with(x)                                                          \
    eer_staging(&(x.instance),                                                 \
                (void *)(uintptr_t)eer_current_land.state.context)             \
        |
#define eer_with(...)                                                          \
    for (union eer_land eer_current_land = eer_land;                           \
         !eer_current_land.state.finished;                                     \
         eer_current_land.state.finished = true)                               \
        for (union eer_land eer_land                                           \
             = {.state = {EVAL(MAP(__eer_with, __VA_ARGS__)) CONTEXT_SAME}};   \
             !eer_land.state.finished; eer_land.state.finished = true)

#define __eer_init(x) eer_staging(&x.instance, (void *)CONTEXT_UPDATED) |
#define eer_init(...)                                                          \
    union eer_land __attribute__((unused)) eer_land;                           \
    eer_land.flags = 0;                                                        \
    goto eer_boot;                                                             \
    eer_boot:                                                                  \
    eer_land.state.context = IF_ELSE(HAS_ARGS(__VA_ARGS__))(                   \
        (EVAL(MAP(__eer_init, __VA_ARGS__)) CONTEXT_UPDATED))(CONTEXT_UPDATED)

#define eer_loop(...)                                                          \
    goto eer_boot;                                                             \
    eer_boot:                                                                  \
    eer_while(__VA_ARGS__)

#define eer_while(...)                                                         \
    for (union eer_land eer_land                                               \
         = {.state = {IF_ELSE(HAS_ARGS(__VA_ARGS__))((EVAL(MAP(                \
                __eer_init, __VA_ARGS__)) CONTEXT_SAME))(CONTEXT_UPDATED)}};   \
         eer_land.state.context;                                               \
         eer_land.state.context = IF_ELSE(HAS_ARGS(__VA_ARGS__))((EVAL(        \
             MAP(__eer_init, __VA_ARGS__)) CONTEXT_SAME))(CONTEXT_UPDATED))


#define eer_terminate                                                          \
    if (!eer_land.state.unmounted)                                             \
        goto eer_boot;
#define eer_halt(code)                                                         \
    if (!eer_land.state.unmounted)                                             \
        goto eer_boot;                                                         \
    {                                                                          \
        return code;                                                           \
    }

#ifdef PROFILING
    #include "profiler.h"
#else
    #define eer_info(...)
    #define eer_debug(...)
    #define eer_print(...)
    #define eer_profiler_count(...)
    #define eer_profiler_count_log(...)
#endif

enum eer_context { CONTEXT_SAME, CONTEXT_UPDATED, CONTEXT_BLOCKED };

union eer_land {
    struct {
        enum eer_context context : 2;
        bool             finished : 1;
        bool             unmounted : 1;
        uint8_t          step : 2;
    } state;
    uint8_t flags;
};

union eer_stage {
    struct {
        enum {
            STAGE_BLOCKED,
            STAGE_RELEASED,
            STAGE_DEFINED,
            STAGE_REACTING,
            STAGE_PREPARED,
            STAGE_UNMOUNTED
        } step : 3;
        bool             updated : 1;
        enum eer_context context : 2;
        uint8_t raise_on : 2; /* TODO: Macros for arch dependent size ARCH_BITS
                                 - 6  */
    } state;
    uint8_t flags;
};

typedef struct eer {
    union eer_stage stage;

    void (*will_mount)(void *instance, void *next_props);

    bool (*should_update)(void *instance, void *next_props);
    void (*will_update)(void *instance, void *next_props);

    void (*release)(void *instance);

    void (*did_mount)(void *instance);
    void (*did_update)(void *instance);
    void (*did_unmount)(void *instance);

#ifdef PROFILING
    PROFILING_STRUCT
#endif
} eer_t;

enum eer_context eer_staging(eer_t *instance, void *next_props);

#ifdef HAL_dbg
    #include <dbg.h>
#endif
#ifdef HAL_atmega328p
    #include <avr.h>
#endif
#ifdef HAL_ch573
    #include <ch573.h>
#endif
#ifdef HAL_ch32v003
    #include <ch32v003.h>
#endif

