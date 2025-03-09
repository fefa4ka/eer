#pragma once

#include "eer_lifecycle.h"

/**
 * @file eer_comp.h
 * @brief Component definition and creation
 * 
 * This file defines macros for creating and managing components in the EER framework.
 */

/**
 * @brief Defines the header for a component of type `Type`.
 * 
 * This macro generates the necessary type definitions and function prototypes
 * for a component. It should be used after defining the props and state structures.
 * 
 * Example:
 * ```c
 * typedef struct {
 *   int value;
 * } MyComponent_props_t;
 * 
 * typedef struct {
 *   int current_value;
 *   bool initialized;
 * } MyComponent_state_t;
 * 
 * eer_header(MyComponent);
 * ```
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

/**
 * @brief Defines the core component structure for a component of type `Type`.
 * 
 * This macro is used internally by the component creation macros.
 * 
 * @param Type The type of the component.
 * @param name The name of the component instance.
 */
#define eer_define_component(Type, name)                                       \
    {                                                                          \
        .stage = {.state = {.step = EER_STAGE_DEFINED, .updated = false,       \
                           .context = EER_CONTEXT_SAME, .raise_on = 0}},       \
        .will_mount = Type##_will_mount,                                       \
        .should_update = Type##_should_update,                                 \
        .will_update = Type##_will_update, .release = Type##_release,          \
        .did_mount = Type##_did_mount, .did_update = Type##_did_update,        \
        .did_unmount = Type##_did_unmount                                      \
    }

/**
 * @brief Defines a pointer to a component of type `Type`.
 * 
 * This macro is used in lifecycle methods to access the component instance.
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

/**
 * @brief Gets a component instance from its props.
 * 
 * @param Type The type of the component.
 * @param props Pointer to the component's props.
 */
#define eer_from_props(Type, props) (Type##_t *)(props - sizeof(eer_t))

/**
 * @brief Gets a component instance from its state.
 * 
 * @param Type The type of the component.
 * @param state Pointer to the component's state.
 */
#define eer_from_state(Type, state)                                            \
    eer_from_props(state - sizeof(Type##_props_t))

/**
 * @brief Gets a pointer to a component's state from an eer_t instance.
 * 
 * @param Type The type of the component.
 * @param instance Pointer to the eer_t instance.
 */
#define eer_instance_state(Type, instance)                                     \
    ((Type##_state_t *)((instance)->state))

/**
 * @brief Gets a pointer to a component's props from an eer_t instance.
 * 
 * @param Type The type of the component.
 * @param instance Pointer to the eer_t instance.
 */
#define eer_instance_props(Type, instance)                                     \
    ((Type##_props_t *)((instance)->props))

/**
 * @defgroup component_creation Component Creation Macros
 * @{
 */

/**
 * @brief Creates a component with specified props.
 * 
 * @param Type The type of the component.
 * @param instance_name The name of the component instance.
 * @param instance_props The initial props for the component.
 */
#define eer_withprops(Type, instance_name, instance_props)                     \
    Type##_t instance_name = {                                                 \
        .instance = eer_define_component(Type, instance_name),                 \
        .props    = instance_props,                                            \
    }

/**
 * @brief Creates a component with specified state.
 * 
 * @param Type The type of the component.
 * @param instance_name The name of the component instance.
 * @param instance_state The initial state for the component.
 */
#define eer_withstate(Type, instance_name, instance_state)                     \
    Type##_t instance_name = {                                                 \
        .instance = eer_define_component(Type, instance_name),                 \
        .state    = instance_state,                                            \
    }

/**
 * @brief Creates a component with default props.
 * 
 * @param Type The type of the component.
 * @param name The name of the component instance.
 */
#define eer(Type, name) eer_withprops(Type, name, _({0}))

/**
 * @brief Creates a component with specified props and state.
 * 
 * @param Type The type of the component.
 * @param instance_name The name of the component instance.
 * @param instance_props The initial props for the component.
 * @param instance_state The initial state for the component.
 */
#define eer_define(Type, instance_name, instance_props, instance_state)        \
    Type##_t instance_name = {                                                 \
        .instance = eer_define_component(Type, instance_name),                 \
        .props    = instance_props,                                            \
        .state    = instance_state,                                            \
    }


/** @} */ // end of component_creation group

/**
 * @brief Helper macro for initializing struct literals.
 * 
 * This macro is used to wrap struct initializers to make them work with
 * the component creation macros.
 * 
 * @param ... The struct initializer.
 */
#define _(...) __VA_ARGS__
