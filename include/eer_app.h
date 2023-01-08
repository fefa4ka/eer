#pragma once

#include "eer.h"

#define ignite eer_init
#define loop   eer_init

#define with  eer_with
#define use   eer_use
#define apply eer_apply
#define react eer_react

#define halt eer_halt

#define pin_t    eer_pin_t
#define pin_mode eer_pin_mode

#define result_t eer_result_t

#define hw(system)        eer_hw_##system
#define hw_pin(port, pin) eer_hw_pin(port, pin)
