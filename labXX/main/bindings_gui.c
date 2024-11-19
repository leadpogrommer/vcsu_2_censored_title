

#include "gui.h"
#include <esp_lvgl_port.h>




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
