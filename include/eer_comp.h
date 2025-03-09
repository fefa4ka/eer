#pragma once

#include "eer_lifecycle.h"

/* This header focuses on component definitions */

/**
 * @brief Defines the header for a component of type `Type`.
 * 
 * @param Type The type of the component.
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
        .stage = {EER_STAGE_DEFINED}, .will_mount = Type##_will_mount,         \
        .should_update = Type##_should_update,                                 \
        .will_update = Type##_will_update, .release = Type##_release,          \
        .did_mount = Type##_did_mount, .did_update = Type##_did_update,        \
    }

/**
 * @brief Defines a pointer to a component of type `Type`.
 * 
 * @param Type The type of the component.
 * @param instance The instance of the component.
 */
#define eer_self(Type, instance) Type##_t *self = instance

/**
 * @brief Returns the value of the specified attribute of the props of the given instance.
 * 
 * @param Type The type of the component.
 * @param instance The instance of the component.
 * @param attribute The attribute of the props to return.
 */
#define eer_prop(Type, instance, attribute)                                    \
    (((Type##_t *)instance)->props.attribute)

/**
 * @brief Returns the value of the specified attribute of the state of the given instance.
 * 
 * @param Type The type of the component.
 * @param instance The instance of the component.
 * @param attribute The attribute of the state to return.
 */
#define eer_state(Type, instance, attribute)                                   \
    (((Type##_t *)instance)->state.attribute)

#define eer_from_props(Type, props) (Type##_t *)(props - sizeof(eer_t))
#define eer_from_state(Type, state)                                            \
    eer_from_props(state - sizeof(Type##_props_t))

#define eer_instance_state(Type, instance)                                     \
    ((Type##_state_t *)((instance)->state))
#define eer_instance_props(Type, instance)                                     \
    ((Type##_props_t *)((instance)->props))

/* Component creation macros */
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
