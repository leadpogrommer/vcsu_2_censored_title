

#include "gui.h"
#include <esp_lvgl_port.h>



static lv_obj_t *get_lv_obj(duk_context *ctx){
    duk_push_this(ctx);
    duk_get_prop_literal(ctx, -1, "ptr");
    auto *res = static_cast<lv_obj_t *>(duk_require_pointer(ctx, -1));
    duk_pop_2(ctx);
    return res;
}

void set_lv_obj(duk_context *ctx, lv_obj_t *obj){
    duk_push_this(ctx);
    duk_push_pointer(ctx, obj);
    duk_put_prop_literal(ctx, -2, "ptr");
    duk_pop(ctx);
}

DUK_BI(lp_bi_lvgl_label_constructor){
    fprintf(stderr, "Label constructor called!\n");
    duk_require_constructor_call(ctx);

    lvgl_port_lock(0);
    lv_obj_t *label = lv_label_create(ct()->screen);
    lvgl_port_unlock();

    set_lv_obj(ctx, label);

    return 0;
}


DUK_BI(lp_bi_lvgl_obj_setter){
    auto value = (short)duk_require_int(ctx, -1);
    auto *obj = get_lv_obj(ctx);
    int magic = duk_get_current_magic(ctx);

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

DUK_BI(lp_bi_lvgl_label_setter){
    char *value= (char *)duk_require_string(ctx, -1);
    auto *obj = get_lv_obj(ctx);
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

DUK_BI(lp_bi_lvgl_button_constructor){
    fprintf(stderr, "Button constructor called!\n");
    duk_require_constructor_call(ctx);

    lvgl_port_lock(0);
    lv_obj_t *btn = lv_btn_create(ct()->screen);
    lv_obj_t *label = lv_label_create(btn);
    lv_obj_center(label);
    lv_obj_set_style_outline_width(btn, 1, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_pad(btn, 2, LV_STATE_FOCUSED);
    lv_obj_set_style_pad_all(btn, 1, 0);
    lv_obj_set_style_pad_gap(btn, 1, 0);
    lv_group_add_obj(ct()->button_group, btn);

    lvgl_port_unlock();

    set_lv_obj(ctx, btn);

    return 0;
}

DUK_BI(lp_bi_lvgl_button_text_setter){
    char *value= (char *)duk_require_string(ctx, -1);
    auto *btn = get_lv_obj(ctx);

    lvgl_port_lock(0);
    auto *label = lv_obj_get_child(btn, 0);
    lv_label_set_text(label, value);
    lv_obj_center(label);
    lvgl_port_unlock();

    return 0;
}