#pragma once

#ifndef __cplusplus
#error "CPP only"
#endif

#include <unordered_set>

#include "freertos/FreeRTOS.h"
#include "lvgl.h"
#include "duktape.h"



#include "unordered_map"

#define DUK_BI(name) extern "C" duk_ret_t name(duk_context *ctx)

class Callback;

typedef struct {
    TaskHandle_t rtos_task;
    // SemaphoreHandle_t sem; // TODO: use this
    lv_obj_t *screen;
    lv_group_t *button_group;
    duk_context *duk_ctx;
    int next_cb_id;
    std::unordered_map<int, Callback*> cbs;
    QueueHandle_t event_queue;
    bool is_js;
    char name[10];
    int tid;
    bool need_js_cleanup;

    // TODO: some logging
} gui_task_t;

static inline gui_task_t *ct(){
    return static_cast<gui_task_t *>(pvTaskGetThreadLocalStoragePointer(nullptr, 0));
}

class Callback {
public:
    Callback(const Callback&) = delete;
    Callback() = delete;
    explicit Callback(gui_task_t *task);
    gui_task_t *task;
    int id;

    virtual void afterRunHook(){}
//    virtual void preDestroyHook(){}

    virtual void run();
    virtual ~Callback();
};


gui_task_t *run_js_task(const char* src);
void kill_task(gui_task_t *task);
