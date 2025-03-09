#pragma once

 #include "interface.h"
 #include <stdbool.h>

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

 /* Common lifecycle hooks */
 #define WILL_MOUNT    eer_will_mount
 #define SHOULD_UPDATE eer_should_update
 #define WILL_UPDATE   eer_will_update
 #define RELEASE       eer_release
 #define DID_MOUNT     eer_did_mount
 #define DID_UPDATE    eer_did_update
 #define DID_UNMOUNT   eer_did_unmount

 /* Skip lifecycle hooks */
 #define WILL_MOUNT_SKIP    eer_will_mount_skip
 #define SHOULD_UPDATE_SKIP eer_should_update_skip
 #define WILL_UPDATE_SKIP   eer_will_update_skip
 #define RELEASE_SKIP       eer_release_skip
 #define DID_MOUNT_SKIP     eer_did_mount_skip
 #define DID_UPDATE_SKIP    eer_did_update_skip
 #define DID_UNMOUNT_SKIP   eer_did_unmount_skip

 /* Hook lifecycle hooks */
 #define WILL_MOUNT_HOOK    eer_will_mount_hook
 #define SHOULD_UPDATE_HOOK eer_should_update_hook
 #define WILL_UPDATE_HOOK   eer_will_update_hook
 #define RELEASE_HOOK       eer_release_hook
 #define DID_MOUNT_HOOK     eer_did_mount_hook
 #define DID_UPDATE_HOOK    eer_did_update_hook
 #define DID_UNMOUNT_HOOK   eer_did_unmount_hook

 /* Lifecycle implementation macros */
 #if __GNUC__
     #define eer_lifecycle_header(Type, stage)                                  \
         static inline void Type##_inline_##stage(                              \
             eer_t *self, Type##_props_t *props, Type##_state_t *state)
 #else
     #define eer_lifecycle_header(Type, stage)                                  \
         inline void Type##_inline_##stage(eer_t *self, Type##_props_t *props,  \
                                           Type##_state_t *state)
 #endif

 #define eer_lifecycle_prepare(Type, instance, stage) //eer_hw_isr_disable()
 #define eer_lifecycle_finish(Type, instance, stage)  //eer_hw_isr_enable()

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

 /* Skip lifecycle implementations */
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

 /* Hook lifecycle implementations */
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
