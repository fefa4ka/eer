#pragma once

#include "magic.h"
#include "interface.h"
#include "eer_comp.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * @file eer.h
 * @brief Core EER Framework definitions
 * @version 0.2.0
 * 
 * This header file defines a set of macros and functions for creating and 
 * managing components in the EER framework. 
 *
 * It provides a set of lifecycle methods that can be implemented to handle 
 * the various stages of a component's lifecycle, such as mounting, updating, 
 * and unmounting. Additionally, it defines macros for accessing a component's 
 * props and state, as well as macros for creating and initializing components.
 */

/**
 * @defgroup version Version Information
 * @{
 */
#define EER_VERSION_MAJOR 0
#define EER_VERSION_MINOR 2
#define EER_VERSION_PATCH 0
#define EER_VERSION_STRING "0.2.0"

/**
 * @brief Check if the current version is at least the specified version
 * @param major Major version number
 * @param minor Minor version number
 * @param patch Patch version number
 * @return true if the current version is at least the specified version
 */
#define EER_VERSION_AT_LEAST(major, minor, patch) \
    ((EER_VERSION_MAJOR > (major)) || \
     (EER_VERSION_MAJOR == (major) && EER_VERSION_MINOR > (minor)) || \
     (EER_VERSION_MAJOR == (major) && EER_VERSION_MINOR == (minor) && EER_VERSION_PATCH >= (patch)))

/** @} */ // end of version group

/* Component interaction macros */
#define eer_apply(Type, name, propsValue)                                      \
    if (EER_CONTEXT_UPDATED == eer_land.state.context                          \
        && (EER_STAGE_RELEASED == name.instance.stage.state.step               \
            || EER_STAGE_DEFINED == name.instance.stage.state.step)) {         \
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
        if (EER_CONTEXT_BLOCKED != eer_land.state.context) {                   \
            &name.instance.stage.state.step = EER_STAGE_REACTING;              \
            eer_staging(&name.instance, &next_props);                          \
        }                                                                      \
    }

#define eer_shut(x)                                                            \
    x.instance.stage.state.step = EER_STAGE_UNMOUNTED;                                    \
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
             = {.state = {EVAL(MAP(__eer_with, __VA_ARGS__)) EER_CONTEXT_SAME}}; \
             !eer_land.state.finished; eer_land.state.finished = true)

/* Event loop macros */
#define __eer_init(x) eer_staging(&x.instance, (void *)EER_CONTEXT_UPDATED) |
#define eer_init(...)                                                          \
    union eer_land __attribute__((unused)) eer_land;                           \
    eer_land.flags = 0;                                                        \
    goto eer_boot;                                                             \
    eer_boot:                                                                  \
    eer_land.state.context = IF_ELSE(HAS_ARGS(__VA_ARGS__))(                   \
        (EVAL(MAP(__eer_init, __VA_ARGS__)) EER_CONTEXT_UPDATED))(EER_CONTEXT_UPDATED)

#define eer_loop(...)                                                          \
    goto eer_boot;                                                             \
    eer_boot:                                                                  \
    eer_while(__VA_ARGS__)

#define eer_while(...)                                                         \
    for (union eer_land eer_land                                               \
         = {.state = {.context = IF_ELSE(HAS_ARGS(__VA_ARGS__))((EVAL(MAP(     \
                __eer_init, __VA_ARGS__)) EER_CONTEXT_SAME))(EER_CONTEXT_UPDATED), \
                     .finished = false, .unmounted = false, .step = 0}}; \
         !eer_land.state.unmounted && eer_land.state.context;                  \
         eer_land.state.context = IF_ELSE(HAS_ARGS(__VA_ARGS__))((EVAL(        \
             MAP(__eer_init, __VA_ARGS__)) EER_CONTEXT_SAME))(EER_CONTEXT_UPDATED))

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
#endif

/* Core types */
enum eer_context { 
    EER_CONTEXT_SAME, 
    EER_CONTEXT_UPDATED, 
    EER_CONTEXT_BLOCKED 
};

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
            EER_STAGE_BLOCKED,
            EER_STAGE_RELEASED,
            EER_STAGE_DEFINED,
            EER_STAGE_REACTING,
            EER_STAGE_PREPARED,
            EER_STAGE_UNMOUNTED
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
