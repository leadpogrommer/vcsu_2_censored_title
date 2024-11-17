#include <sys/cdefs.h>
#include "freertos/FreeRTOS.h"
#include "rotary_encoder.h"
#include <esp_lvgl_port.h>
#include <lvgl.h>
#include "esp_log.h"


QueueHandle_t encoder_raw_event_queue;
rotary_encoder_info_t encoder_info = { 0 };

void init_encoder(){
    // setup encoder
    ESP_ERROR_CHECK(gpio_install_isr_service(0));


    ESP_ERROR_CHECK(rotary_encoder_init(&encoder_info, 7, 6));
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&encoder_info, false));

    encoder_raw_event_queue = rotary_encoder_create_queue();
    ESP_ERROR_CHECK(rotary_encoder_set_queue(&encoder_info, encoder_raw_event_queue));
}



static lv_obj_t *create_sample_obj(char *s) {
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_obj_t *lbl = lv_label_create(screen);
    lv_label_set_text(lbl, s);
    lv_obj_set_width(lbl, 128);
    lv_obj_set_height(lbl, 64);
    lv_obj_set_x(lbl, 30);
    lv_obj_set_y(lbl, 30);
    return screen;
}

#define TAG "encoder"

_Noreturn void encoder_run_demo(lv_obj_t **screens, int n){
//    lvgl_port_lock(0);
//    lv_obj_t *screens[3] = {
//            create_sample_obj("1 First"),
//            create_sample_obj("2 Second"),
//            create_sample_obj("3 Third"),
//    };
//    lvgl_port_unlock();
    int cscr = 0;

    lvgl_port_lock(0);
    lv_scr_load_anim(screens[cscr], LV_SCR_LOAD_ANIM_MOVE_RIGHT , 500, 0, false);
    lvgl_port_unlock();


    ESP_LOGI(TAG, "Encoder demo loop starting...");
    while (1) {
        rotary_encoder_event_t event = { 0 };
        if (xQueueReceive(encoder_raw_event_queue, &event, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "Event: position %ld, direction %s", event.state.position,
                     event.state.direction ? (event.state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? "CW" : "CCW") : "NOT_SET");
            rotary_encoder_direction_t dir = event.state.direction;
            if (dir == ROTARY_ENCODER_DIRECTION_NOT_SET) continue;
            cscr += dir == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? 1 : -1;
            if(cscr < 0) cscr = n-1;
            cscr %= n;
            lvgl_port_lock(0);
            lv_scr_load_anim(screens[cscr], dir != ROTARY_ENCODER_DIRECTION_CLOCKWISE ? LV_SCR_LOAD_ANIM_MOVE_RIGHT : LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, false);
            lvgl_port_unlock();


        } else {
            ESP_LOGE(TAG, "fuck?");
        }
    }
}