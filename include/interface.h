#pragma once

typedef enum eer_result {
    OK = 0,
    ERROR_UNKNOWN,
    ERROR_BUFFER_FULL,
    ERROR_BUFFER_EMPTY,
    ERROR_BUFFER_BUSY
} eer_result_t;

typedef struct eer_callback {
    eer_result_t (*method)(void *argument, void *trigger);
    void *argument;
} eer_callback_t;
