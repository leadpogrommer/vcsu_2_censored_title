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

static inline gui_task_t *ct(){
    return pvTaskGetThreadLocalStoragePointer(NULL, 0);
}


duk_ret_t lp_bi_lvgl_object_constructor(duk_context *ctx){
    printf("Object constructor called!\n");
    duk_require_constructor_call(ctx);
    duk_push_this(ctx);
    return 1;
}
duk_ret_t lp_bi_lvgl_label_constructor(duk_context *ctx){
    fprintf(stderr, "Label constructor called!\n");
    duk_require_constructor_call(ctx);

    lvgl_port_lock(0);
    lv_obj_t *label = lv_label_create(ct()->screen);
    lvgl_port_unlock();

    duk_push_this(ctx);
    duk_push_string(ctx, "ptr");
    duk_push_pointer(ctx, label);
    duk_put_prop(ctx, -3);

    return 1;
}


duk_ret_t lp_bi_lvgl_obj_setter(duk_context *ctx){
    duk_int_t value = duk_require_int(ctx, -1);
    duk_push_this(ctx);
    duk_push_string(ctx, "ptr");
    duk_get_prop(ctx, -2);
    lv_obj_t *obj = duk_require_pointer(ctx, -1);
//    duk_get_prop();
    int magic = duk_get_current_magic(ctx);
//    int val = duk_requi

    lvgl_port_lock(0);
    switch (magic) {
        case 0:
            lv_obj_set_x(obj, value);
            break;
        case 1:
            lv_obj_set_y(obj, value);
            break;
        case 2:
            lv_obj_set_width(obj, value);
            break;
        case 3:
            lv_obj_set_height(obj, value);
            break;
        default:
            printf("Error: unknown magic in object setter: %d\n", magic);
    }
    lvgl_port_unlock();

//    printf("Setter called! %d = %d\n", magic, value);
    return 0;
}

duk_ret_t lp_bi_lvgl_label_setter(duk_context *ctx){
    char *value= duk_require_string(ctx, -1);
    duk_push_this(ctx);
    duk_push_string(ctx, "ptr");
    duk_get_prop(ctx, -2);
    lv_obj_t *obj = duk_require_pointer(ctx, -1);
    int magic = duk_get_current_magic(ctx);

    lvgl_port_lock(0);
    switch (magic) {
        case 0:
            lv_label_set_text(obj, value);
            break;

        default:
            printf("Error: unknown magic in object setter: %d\n", magic);
    }
    lvgl_port_unlock();

//    printf("Setter called! %d = %s\n", magic, value);
    return 0;
}
