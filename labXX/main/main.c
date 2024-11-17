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
#include "main.h"



#define TAG "main"


void app_main() {
    printf("Hello world!\n");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    init_lcd();
    nvs_flash_init();
//    connect_to_wifi();

    init_encoder();




    encoder_run_demo();
}
