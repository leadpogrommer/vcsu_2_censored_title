#include <stdio.h>

#include "esp_event.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "freertos/FreeRTOS.h"

#include "input.h"
#include "gui.h"
#include "display.h"
#include "taskmgr.h"

#define TAG "main"

struct xTASK_STATUS debug_tasks[32];
char dbg_buffer[1024*10];

extern "C" void app_main() {
    printf("Hello world!\n");
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    init_lcd();
    nvs_flash_init();
//    connect_to_wifi();
    taskmgr_init();
    start_input_manager();


    taskmgr_run_js("flyer", "var flyer = new LVGLLabel();\nvar stats = new LVGLLabel();\n\nflyer.text = \"X\"\nvar x = 0;\nvar y = 0;\nvar dirx = 1;\nvar diry = 1\n\n\nfunction f(){\n    x += dirx;\n    y += diry;\n    if(x > 118 || x == 0) dirx = - dirx;\n    if(y > 54 || y == 0) diry = - diry;\n    flyer.x = x;\n    flyer.y = y;\n    stats.text = \"x=\"+x+\"\\ny=\"+y;\n    setTimeout(f, 100);\n}\n\nf()");
    taskmgr_run_js("btns", "var btn = new LVGLButton();\nbtn.x = 10;\nbtn.y = 10;\nbtn.text = 'B 1'\n\nvar btn = new LVGLButton();\nbtn.x = 10;\nbtn.y = 25;\nbtn.text = 'B 2'\n\nvar btn = new LVGLButton();\nbtn.x = 10;\nbtn.y = 40;\nbtn.text = 'B 3'\n\nvar lbl = new LVGLLabel();\nlbl.x = 100;\nlbl.y = 45;\nlbl.text = '^b^'");
//    gui_task_t *task0 = run_js_task("var btn = new LVGLButton();\nbtn.x = 10;\nbtn.y = 10;\nbtn.text = 'B 1'\n\nvar btn = new LVGLButton();\nbtn.x = 10;\nbtn.y = 25;\nbtn.text = 'B 2'\n\nvar btn = new LVGLButton();\nbtn.x = 10;\nbtn.y = 40;\nbtn.text = 'B 3'\n\nvar lbl = new LVGLLabel();\nlbl.x = 100;\nlbl.y = 45;\nlbl.text = '^b^'");
//    gui_task_t *task1 = run_js_task("var flyer = new LVGLLabel();\nvar stats = new LVGLLabel();\n\nflyer.text = \"X\"\nvar x = 0;\nvar y = 0;\nvar dirx = 1;\nvar diry = 1\n\n\nfunction f(){\n    x += dirx;\n    y += diry;\n    if(x > 118 || x == 0) dirx = - dirx;\n    if(y > 54 || y == 0) diry = - diry;\n    flyer.x = x;\n    flyer.y = y;\n    stats.text = \"x=\"+x+\"\\ny=\"+y;\n    setTimeout(f, 100);\n}\n\nf()");
//    gui_task_t *task2 = run_js_task("var lbl = new LVGLLabel();\nvar  tick = 0;\n\nfunction f() {\n    lbl.text = \"Tick \" + (tick++);\n    setTimeout(f, 1000);\n}\n\nf();\n");
//    lv_obj_t *screens[] = {task0->screen};
//    encoder_run_demo(screens, 1);
//    run_js_task("\nprint('Timeout sort!')\nfor(var i = 5; i >= 0; i--){\n    setTimeout(function (e) {\n        return function () {\n            print(\"Timeout\", e)\n        }\n    }(i), i*1000);\n}\n");
//    lv_scr_load(task0->screen);
//    gui_task_t *tasks[] = {task0};
//    encoder_run_demo(tasks, 1);
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
