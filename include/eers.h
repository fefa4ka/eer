#pragma once

#include "eer_common.h"
#include "eer_app.h"
#include "eer_comp.h"

/**
 * @file eers.h
 * @brief Simplified EER Framework header
 * 
 * This header provides a simplified interface to the EER framework
 * by including all necessary components and providing convenient aliases.
 */

/* Component creation shortcuts */
#define Component(Type, name, props) eer_withprops(Type, name, props)
#define ComponentWithState(Type, name, props, state) eer_define(Type, name, props, state)

/* Lifecycle shortcuts */
#define Mount(Type, name) eer_staging(&name.instance, (void *)EER_CONTEXT_UPDATED)
#define Unmount(Type, name) eer_shut(name)

/* Event loop shortcuts */
#define App(...) loop(__VA_ARGS__)
#define Run(...) loop(__VA_ARGS__)

/* This is a simplified header that includes all necessary components */
