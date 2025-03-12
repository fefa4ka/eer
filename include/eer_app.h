#pragma once

#include "eer_common.h"

/**
 * @file eer_app.h
 * @brief Application-level macros for the EER framework
 * 
 * This file provides macros for creating and managing EER applications.
 */

/* Event loop macros */
#define ignite eer_init
#define loop   eer_loop
#define app    eer_loop

/* Control flow macros */
#define halt      eer_halt
#define terminate eer_terminate
#define exit_app  eer_halt

/* Component lifecycle helpers */
#define is_mounted(component) ((component).instance.stage.state.step == EER_STAGE_RELEASED)
#define is_updating(component) ((component).instance.stage.state.step == EER_STAGE_PREPARED)
#define is_unmounted(component) ((component).instance.stage.state.step == EER_STAGE_UNMOUNTED || \
                                (component).instance.stage.state.step == EER_STAGE_BLOCKED)

/* Application exit helper */
#define exit_when(condition) if (condition) { eer_land.state.unmounted = true; }
