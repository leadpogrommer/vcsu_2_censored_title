#include <cstdio>

#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"

#include "input.h"
#include "gui.h"
#include "display.h"
#include "taskmgr.h"
#include "network.h"

#define TAG "main"

struct xTASK_STATUS debug_tasks[32];
char dbg_buffer[1024*10];

// TO DOS for fw
// TODO: task termination
// TODO: demo: matrix
// TODO: send telemetry (task logs, free heap, rtos task stats)

extern "C" void app_main() {
    printf("Hello world!\n");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    init_lcd();
    nvs_flash_init();
    connect_to_wifi();
    taskmgr_init();
    start_input_manager();


//    taskmgr_run_js("sb", "var lbl = new LVGLLabel();\n\nvar b = new LVGLButton(function f(){\n   print('button pressed'); \n});\nb.x = 70;\nb.y = 15;\nb.text = 'Btn';\n\nvar sb = new LVGLSpinbox(function (v){\n    lbl.text = 'Value is ' + v;\n}, 10, 90, 7, 2);\n\nsb.y = 30;\nsb.x = 15;\nsb.w = 40;");
//    taskmgr_run_js("flyer", "var flyer = new LVGLLabel();\nvar stats = new LVGLLabel();\n\nflyer.text = \"X\"\nvar x = 0;\nvar y = 0;\nvar dirx = 1;\nvar diry = 1\n\n\nfunction f(){\n    x += dirx;\n    y += diry;\n    if(x > 118 || x == 0) dirx = - dirx;\n    if(y > 54 || y == 0) diry = - diry;\n    flyer.x = x;\n    flyer.y = y;\n    stats.text = \"x=\"+x+\"\\ny=\"+y;\n    setTimeout(f, 100);\n}\n\nf()");
//    taskmgr_run_js("btns", "\nvar cntr = 0;\n\nvar lbl = new LVGLLabel();\nlbl.x = 100;\nlbl.y = 45;\nlbl.text = '0';\n\nfunction upd(){\n    lbl.text = '' + cntr;\n}\n\nfunction inc(){\n    cntr++;\n    upd();\n}\nfunction dec(){\n    cntr--;\n    upd();\n}\nfunction reset(){\n    cntr = 0;\n    upd();\n}\n\nvar btn = new LVGLButton(inc);\nbtn.x = 10;\nbtn.y = 10;\nbtn.text = '+'\n\nvar btn = new LVGLButton(dec);\nbtn.x = 10;\nbtn.y = 25;\nbtn.text = '-'\n\nvar btn = new LVGLButton(reset);\nbtn.x = 10;\nbtn.y = 40;\nbtn.text = '0'\n\n");

//    while (1) {
//        vTaskDelay(1000/portTICK_PERIOD_MS);
//
//        vTaskGetRunTimeStats(dbg_buffer);
//        printf("%s\n", dbg_buffer);
//
//        uint32_t total_runtime;
////        int total_tasks = uxTaskGetSystemState(debug_tasks, 32, &total_runtime);
////        printf("--------- TASK DUMP ---------\n");
////        for(int i = 0; i < total_tasks; i++){
////            printf("[%d] %s (bp = %d, cp = %d, runtime = %lu)\n", debug_tasks[i].xTaskNumber, debug_tasks[i].pcTaskName, debug_tasks[i].uxBasePriority, debug_tasks[i].uxCurrentPriority, debug_tasks[i].ulRunTimeCounter);
////        }
//    }
}
