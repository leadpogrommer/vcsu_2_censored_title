#pragma once
#include "freertos/FreeRTOS.h"
#include "lvgl.h"
#include "duktape.h"

#ifndef __cplusplus
#error "CPP only"
#endif

#define DUK_BI(name) extern "C" duk_ret_t name(duk_context *ctx)

typedef struct {
    TaskHandle_t rtos_task;
    SemaphoreHandle_t sem; // TODO: use this
    lv_obj_t *screen;
    duk_context *duk_ctx;
    // TODO: input event queue
    // TODO: some logging
} gui_task_t;

gui_task_t *run_js_task(const char* src);

static inline gui_task_t *ct(){
    return static_cast<gui_task_t *>(pvTaskGetThreadLocalStoragePointer(nullptr, 0));
}