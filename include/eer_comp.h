
#pragma once

#include "eer.h"

#define WILL_MOUNT    eer_will_mount
#define SHOULD_UPDATE eer_should_update
#define WILL_UPDATE   eer_will_update
#define RELEASE       eer_release
#define DID_MOUNT     eer_did_mount
#define DID_UPDATE    eer_did_update
#define DID_UNMOUNT   eer_did_unmount

#define WILL_MOUNT_SKIP    eer_will_mount_skip
#define SHOULD_UPDATE_SKIP eer_should_update_skip
#define WILL_UPDATE_SKIP   eer_will_update_skip
#define RELEASE_SKIP       eer_release_skip
#define DID_MOUNT_SKIP     eer_did_mount_skip
#define DID_UPDATE_SKIP    eer_did_update_skip
#define DID_UNMOUNT_SKIP   eer_did_unmount_skip

#define WILL_MOUNT_HOOK    eer_will_mount_hook
#define SHOULD_UPDATE_HOOK eer_should_update_hook
#define WILL_UPDATE_HOOK   eer_will_update_hook
#define RELEASE_HOOK       eer_release_hook
#define DID_MOUNT_HOOK     eer_did_mount_hook
#define DID_UPDATE_HOOK    eer_did_update_hook
#define DID_UNMOUNT_HOOK   eer_did_unmount_hook

#define pin_t    eer_pin_t
#define pin_mode eer_pin_mode

#define result_t eer_result_t

#define hw(system)        eer_hw_##system
#define hw_pin(port, pin) eer_hw_pin(port, pin)
