#pragma once

#include <stdbool.h>
#include <stdint.h>

#define bit_value(data, bit) ((data >> bit) & 1) // Return Data.Y value
#define bit_set(data, bit)   data |= (1 << bit)  // Set Data.Y to 1
#define bit_flip(data, bit)  data ^= (1 << bit)  // Set Data.Y to ! Data.Y
#define bit_clear(data, bit) data &= ~(1 << bit) // Clear Data.Y to 0
