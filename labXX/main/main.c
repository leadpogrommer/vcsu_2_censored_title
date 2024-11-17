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
#include "gui.h"


#define TAG "main"


void app_main() {
    printf("Hello world!\n");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    init_lcd();
    nvs_flash_init();
//    connect_to_wifi();

    init_encoder();


    gui_task_t *task = run_js_task("var flyer = new LVGLLabel();\nvar stats = new LVGLLabel();\n\nflyer.text = \"X\"\nvar x = 0;\nvar y = 0;\nvar dirx = 1;\nvar diry = 1\n\n\nfunction do_update(){\n    x += dirx;\n    y += diry;\n    if(x > 118 || x == 0) dirx = - dirx;\n    if(y > 54 || y == 0) diry = - diry;\n    flyer.x = x;\n    flyer.y = y;\n    stats.text = \"x=\"+x+\"\\ny=\"+y;\n}");
    lvgl_port_lock(0);
    lv_scr_load(task->screen);
    lvgl_port_unlock();


//    encoder_run_demo();
}
