#include <gui.h>

#include <esp_lvgl_port.h>
void js_task_loop(gui_task_t *task){
    vTaskSetThreadLocalStoragePointer(NULL, 0, task);

    // run pushed task source
    duk_eval(task->duk_ctx);

    // TODO: replace this with event loop
    while (1){
        // TODO: check this
        duk_bool_t res = duk_get_global_string(task->duk_ctx, "do_update");
        duk_call(task->duk_ctx, 0);
        duk_pop(task->duk_ctx);
        vTaskDelay(100/portTICK_PERIOD_MS);
    }
}


gui_task_t *run_js_task(char* src){
    gui_task_t *task = malloc(sizeof(gui_task_t));
    lvgl_port_lock(0);
    task->screen = lv_obj_create(NULL);
    lvgl_port_unlock();

    task->sem = xSemaphoreCreateMutex();
    task->duk_ctx = duk_create_heap_default();

    duk_push_string(task->duk_ctx, src);
    xTaskCreate((TaskFunction_t) js_task_loop, "Js task", 10000, task, 1, &task->rtos_task);

    return task;
}


// TODO: move lvgl bindings to separate file

