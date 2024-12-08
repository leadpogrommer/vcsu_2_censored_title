#include <gui.h>

#include <esp_lvgl_port.h>
#include <esp_log.h>

static int next_tid = 1;

[[noreturn]] static void js_task_loop(gui_task_t *task){
    vTaskSetThreadLocalStoragePointer(nullptr, 0, task);

    duk_context  *ctx = task->duk_ctx;

    // init callback store
    duk_push_global_stash(ctx);
    duk_push_object(ctx);
    duk_put_prop_literal(ctx, -2, "cbs");
    duk_pop(ctx);

    // run pushed task source
    duk_eval(task->duk_ctx);

    while (1){
        Callback *cb;
        xQueueReceive(task->event_queue, &cb, portMAX_DELAY);
        cb->run();
    }
}

static void js_task_cleanup(gui_task_t *task){
    auto ctx = task->duk_ctx;

    // TODO: lock something
    // destroy all callbacks
    // TODO: figure out if it's ok to delete callbacks while we are in a callback
    // Can't iterate normally: destructor will remove cb from map
    while (!task->cbs.empty()){
        auto *cb = task->cbs.begin()->second;
        delete cb;
    }

    // destroy event queue
    vQueueDelete(task->event_queue);

    //  destroy js heap
    duk_destroy_heap(ctx);

    // destroy RTOS task
    vTaskDelete(task->rtos_task);
}

static void task_fatal_error_handler(gui_task_t *task, const char *msg){
    ESP_LOGE(task->name, "JS Task crashed: %s", msg);

    lvgl_port_lock(0);
    for (int i = 0; i < lv_obj_get_child_cnt(task->screen); i++){
        lv_obj_del_async(lv_obj_get_child(task->screen, i));
    }

    auto lbl = lv_label_create(task->screen);
    lv_label_set_text(lbl, "Task CRASHED!");
    lv_obj_set_x(lbl, 0);
    lv_obj_set_y(lbl, 0);

    lbl = lv_label_create(task->screen);
    lv_label_set_text(lbl, msg);
    lv_label_set_long_mode(lbl, LV_LABEL_LONG_WRAP);
    lv_obj_set_x(lbl, 0);
    lv_obj_set_y(lbl, 15);
    lv_obj_set_width(lbl, 128);

    lvgl_port_unlock();

    js_task_cleanup(task);
}

gui_task_t *run_js_task(const char* src){
    auto *task = new gui_task_t;
    lvgl_port_lock(0);
    task->screen = lv_obj_create(NULL);
    task->button_group = lv_group_create();
    task->tid = next_tid++;
    lvgl_port_unlock();

    task->sem = xSemaphoreCreateMutex();
    task->duk_ctx = duk_create_heap(nullptr, nullptr, nullptr, task,
                                    reinterpret_cast<duk_fatal_function>(task_fatal_error_handler));
    task->event_queue = xQueueCreate(10, sizeof(size_t));

    task->next_cb_id = 0;
    task->is_js = true;

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

