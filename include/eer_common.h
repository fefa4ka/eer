#pragma once

#include "eer.h"

/* Common type definitions */
#define result_t eer_result_t
#define pin_t    eer_pin_t
#define pin_mode eer_pin_mode

/* Common application functions */
#define react    eer_react
#define with     eer_with
#define apply    eer_apply
#define use      eer_use

/* Common hardware abstractions */
#define hw(system)        eer_hw_##system
#define hw_pin(port, pin) eer_hw_pin(port, pin)
