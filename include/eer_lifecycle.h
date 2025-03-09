#pragma once

#include "interface.h"
#include <stdbool.h>

/**
 * @file eer_lifecycle.h
 * @brief Component lifecycle management
 * 
 * This file defines the lifecycle hooks and implementation macros for EER components.
 * The lifecycle follows this state machine:
 * 
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

/**
 * @defgroup lifecycle_hooks Component Lifecycle Hooks
 * @{
 */

/** @brief Called before a component is mounted. Use this to initialize state based on props. */
#define WILL_MOUNT    eer_will_mount

/** @brief Determines if a component should update. Return true to update, false to skip. */
#define SHOULD_UPDATE eer_should_update

/** @brief Called before a component updates. Use this to prepare for the update. */
#define WILL_UPDATE   eer_will_update

/** @brief The main update function. Apply props to state here. */
#define RELEASE       eer_release

/** @brief Called after a component has mounted. Use for initialization that requires the component to be fully mounted. */
#define DID_MOUNT     eer_did_mount

/** @brief Called after a component has updated. Use this for side effects. */
#define DID_UPDATE    eer_did_update

/** @brief Called when a component is unmounted. Use this for cleanup. */
#define DID_UNMOUNT   eer_did_unmount

/** @} */ // end of lifecycle_hooks group

/**
 * @defgroup lifecycle_skip Skip Lifecycle Hooks
 * @brief Default implementations that skip lifecycle hooks
 * @{
 */
#define WILL_MOUNT_SKIP    eer_will_mount_skip
#define SHOULD_UPDATE_SKIP eer_should_update_skip
#define WILL_UPDATE_SKIP   eer_will_update_skip
#define RELEASE_SKIP       eer_release_skip
#define DID_MOUNT_SKIP     eer_did_mount_skip
#define DID_UPDATE_SKIP    eer_did_update_skip
#define DID_UNMOUNT_SKIP   eer_did_unmount_skip
/** @} */ // end of lifecycle_skip group


/* Lifecycle implementation macros - internal use only */

/**
 * @brief Define the header for a lifecycle method
 * @param Type The component type
 * @param stage The lifecycle stage
 */
#if __GNUC__
    #define eer_lifecycle_header(Type, stage)                                  \
        static inline void Type##_inline_##stage(                              \
            eer_t *self, Type##_props_t *props, Type##_state_t *state)
#else
    #define eer_lifecycle_header(Type, stage)                                  \
        inline void Type##_inline_##stage(eer_t *self, Type##_props_t *props,  \
                                          Type##_state_t *state)
#endif

/**
 * @brief Prepare for a lifecycle method (e.g., disable interrupts)
 * @param Type The component type
 * @param instance The component instance
 * @param stage The lifecycle stage
 */
#define eer_lifecycle_prepare(Type, instance, stage) //eer_hw_isr_disable()

/**
 * @brief Finish a lifecycle method (e.g., enable interrupts)
 * @param Type The component type
 * @param instance The component instance
 * @param stage The lifecycle stage
 */
#define eer_lifecycle_finish(Type, instance, stage)  //eer_hw_isr_enable()

/**
 * @brief Define a lifecycle method that doesn't take next_props
 * @param Type The component type
 * @param stage The lifecycle stage
 */
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

/**
 * @brief Helper macro to access next_props in update cycle methods
 * @param Type The component type
 * @param instance The component instance
 */
#define eer_selfnext(Type, instance) Type##_props_t *next_props = next_props_ptr

/**
 * @brief Define the header for an update cycle method
 * @param Type The component type
 * @param stage The lifecycle stage
 * @param returnType The return type of the method
 */
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

/**
 * @brief Define an update cycle method that takes next_props
 * @param Type The component type
 * @param stage The lifecycle stage
 * @param returnType The return type of the method
 */
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

/**
 * @brief Define the will_mount lifecycle method
 * @param Type The component type
 * 
 * This method is called before a component is mounted.
 * It initializes the component's state based on props.
 */
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

/**
 * @brief Define the will_update lifecycle method
 * @param Type The component type
 * 
 * This method is called before a component updates.
 * It prepares the component for the update.
 */
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

/**
 * @brief Define the release lifecycle method
 * @param Type The component type
 * 
 * This method is called to apply props to state.
 */
#define eer_release(Type)       eer_lifecycle(Type, release)

/**
 * @brief Define the should_update lifecycle method
 * @param Type The component type
 * 
 * This method determines if a component should update.
 * Return true to update, false to skip.
 */
#define eer_should_update(Type) eer_updatecycle(Type, should_update, bool)

/**
 * @brief Define the did_mount lifecycle method
 * @param Type The component type
 * 
 * This method is called after a component has mounted.
 */
#define eer_did_mount(Type)     eer_lifecycle(Type, did_mount)

/**
 * @brief Define the did_update lifecycle method
 * @param Type The component type
 * 
 * This method is called after a component has updated.
 */
#define eer_did_update(Type)    eer_lifecycle(Type, did_update)

/**
 * @brief Define the did_unmount lifecycle method
 * @param Type The component type
 * 
 * This method is called when a component is unmounted.
 */
#define eer_did_unmount(Type)   eer_lifecycle(Type, did_unmount)

/* Skip lifecycle implementations */

/**
 * @brief Skip a lifecycle method
 * @param Type The component type
 * @param stage The lifecycle stage
 */
#define eer_lifecycle_skip(Type, stage)                                        \
    void Type##_##stage(void *instance)                                        \
    {                                                                          \
    }

/**
 * @brief Skip an update cycle method
 * @param Type The component type
 * @param stage The lifecycle stage
 * @param return_type The return type of the method
 */
#define eer_updatecycle_skip(Type, stage, return_type)                         \
    return_type Type##_##stage(void *instance, void *next_props_ptr)           \
    {                                                                          \
    }

/**
 * @brief Skip the will_mount lifecycle method but still copy props
 * @param Type The component type
 */
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

/**
 * @brief Skip the will_update lifecycle method but still copy props
 * @param Type The component type
 */
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

/**
 * @brief Skip the release lifecycle method
 * @param Type The component type
 */
#define eer_release_skip(Type) eer_lifecycle_skip(Type, release)

/**
 * @brief Skip the should_update lifecycle method but always return true
 * @param Type The component type
 */
#define eer_should_update_skip(Type)                                           \
    bool Type##_should_update(void *instance, void *next_props_ptr)            \
    {                                                                          \
        return true;                                                           \
    }

/**
 * @brief Skip the did_mount lifecycle method
 * @param Type The component type
 */
#define eer_did_mount_skip(Type)   eer_lifecycle_skip(Type, did_mount)

/**
 * @brief Skip the did_update lifecycle method
 * @param Type The component type
 */
#define eer_did_update_skip(Type)  eer_lifecycle_skip(Type, did_update)

/**
 * @brief Skip the did_unmount lifecycle method
 * @param Type The component type
 */
#define eer_did_unmount_skip(Type) eer_lifecycle_skip(Type, did_unmount)

