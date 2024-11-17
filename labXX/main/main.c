#include <sys/cdefs.h>
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

struct xTASK_STATUS debug_tasks[32];
char dbg_buffer[1024*10];

_Noreturn void app_main() {
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

    while (1) {
        vTaskDelay(1000/portTICK_PERIOD_MS);

        vTaskGetRunTimeStats(dbg_buffer);
        printf("%s\n", dbg_buffer);

        uint32_t total_runtime;
//        int total_tasks = uxTaskGetSystemState(debug_tasks, 32, &total_runtime);
//        printf("--------- TASK DUMP ---------\n");
//        for(int i = 0; i < total_tasks; i++){
//            printf("[%d] %s (bp = %d, cp = %d, runtime = %lu)\n", debug_tasks[i].xTaskNumber, debug_tasks[i].pcTaskName, debug_tasks[i].uxBasePriority, debug_tasks[i].uxCurrentPriority, debug_tasks[i].ulRunTimeCounter);
//        }
    }


//    encoder_run_demo();
}