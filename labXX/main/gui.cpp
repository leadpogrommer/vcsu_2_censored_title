#include <gui.h>

#include <esp_lvgl_port.h>
void js_task_loop(gui_task_t *task){
    vTaskSetThreadLocalStoragePointer(NULL, 0, task);

    duk_context  *ctx = task->duk_ctx;

    // init callback store
    duk_push_global_stash(ctx);
    duk_push_object(ctx);
    duk_put_prop_literal(ctx, -2, "cbs");
    duk_pop(ctx);

//    duk_eval_string(ctx, "function _store_cb(cb, stash, ptr, idx){stash['cbs'] = {ptr, cb};}");
//    duk_eval(ctx);

    // run pushed task source
    duk_eval(task->duk_ctx);

//    while (1){
//        duk_bool_t res = duk_get_global_string(task->duk_ctx, "do_update");
//        duk_call(task->duk_ctx, 0);
//        duk_pop(task->duk_ctx);
//        vTaskDelay(100/portTICK_PERIOD_MS);
//    }

    while (1){
        Callback *cb;
        xQueueReceive(task->event_queue, &cb, portMAX_DELAY);
        cb->run();
    }
}


gui_task_t *run_js_task(const char* src){
    auto *task = new gui_task_t;
    lvgl_port_lock(0);
    task->screen = lv_obj_create(NULL);
    lvgl_port_unlock();

    task->sem = xSemaphoreCreateMutex();
    task->duk_ctx = duk_create_heap_default();
    task->event_queue = xQueueCreate(10, sizeof(size_t));

    task->next_cb_id = 0;

    duk_push_string(task->duk_ctx, src);
    xTaskCreate((TaskFunction_t) js_task_loop, "Js task", 10000, task, 1, &task->rtos_task);

    return task;
}


Callback::Callback(gui_task_t *task) : task(task) {
    assert(task == ct());
    id = task->next_cb_id++;
    duk_context *ctx =task->duk_ctx;
    // we expect that callable is on top of the stack
//    duk_dup_top(ctx);
    duk_push_global_stash(ctx);
    // [... cb stash  ]
    duk_get_prop_literal(ctx, -1, "cbs");
    // [... cb stash cbs ]
    duk_dup(ctx, -3);
    // [... cb stash cbs cb ]
    duk_put_prop_index(ctx, -2, id);
    // [... cb stash cbs ]
    duk_pop(ctx);
    duk_pop(ctx);

    task->cbs[id] = this;
}

void Callback::run() {
    duk_context *ctx =task->duk_ctx;
    duk_push_global_stash(ctx);
    duk_get_prop_literal(ctx, -1, "cbs");
    duk_get_prop_index(ctx, -1, id);
    duk_call(ctx, 0);
    duk_pop_3(ctx);
    afterRunHook();
}

Callback::~Callback() {
    duk_context *ctx =task->duk_ctx;
    duk_push_global_stash(ctx);
    duk_get_prop_literal(ctx, -1, "cbs");
    duk_del_prop_index(ctx, -1, id);
    duk_pop_2(ctx);

    task->cbs.erase(id);
}

