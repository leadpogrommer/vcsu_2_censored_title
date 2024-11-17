#include <stdio.h>
#include <esp_lcd_panel_vendor.h>
// #include <driver/i2c_master.h>
#include "driver/i2c_master.h"
#include "esp_lcd_panel_ssd1306.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io_interface.h"
#include <esp_lvgl_port.h>
#include <lvgl.h>
#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"
#include "rotary_encoder.h"
#include "labXX.h"



#define TAG "main"


lv_obj_t *create_sample_obj(char *s) {
    lv_obj_t *screen = lv_obj_create(NULL);
    lv_label_t *lbl = lv_label_create(screen);
    lv_label_set_text(lbl, s);
    lv_obj_set_width(lbl, 128);
    lv_obj_set_height(lbl, 64);
    lv_obj_set_x(lbl, 30);
    lv_obj_set_y(lbl, 30);
    return screen;
}

void app_main() {
    printf("Hello world!\n");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    init_lcd();
    nvs_flash_init();
    connect_to_wifi();
    // TODO: move this to another file
    // create screens

    lvgl_port_lock(0);
    lv_obj_t *screens[3] = {
        create_sample_obj("First"),
        create_sample_obj("Second"),
        create_sample_obj("Third"),
    };
    lvgl_port_unlock();
    int cscr = 2;

    // setup encoder
    ESP_ERROR_CHECK(gpio_install_isr_service(0));

    rotary_encoder_info_t info = { 0 };
    ESP_ERROR_CHECK(rotary_encoder_init(&info, 7, 6));
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&info, false));

    QueueHandle_t event_queue = rotary_encoder_create_queue();
    ESP_ERROR_CHECK(rotary_encoder_set_queue(&info, event_queue));

    while (1) {
        rotary_encoder_event_t event = { 0 };
        if (xQueueReceive(event_queue, &event, portMAX_DELAY) == pdTRUE) {
            ESP_LOGI(TAG, "Event: position %ld, direction %s", event.state.position,
                     event.state.direction ? (event.state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? "CW" : "CCW") : "NOT_SET");
            rotary_encoder_direction_t dir = event.state.direction;
            if (dir == ROTARY_ENCODER_DIRECTION_NOT_SET) continue;
            cscr += dir == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? 1 : -1;
            if(cscr < 0) cscr = 2;
            cscr %= 3;
            lvgl_port_lock(0);
            lv_scr_load_anim(screens[cscr], dir != ROTARY_ENCODER_DIRECTION_CLOCKWISE ? LV_SCR_LOAD_ANIM_MOVE_RIGHT : LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, false);
            lvgl_port_unlock();
            

        } else {
            ESP_LOGE(TAG, "fuck?");
        }
    }
    

    
}
