
#include "network.h"
#include "gui.h"
#include "taskmgr.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

#include <string>

void telemetry_task(){
    while(true){
        {
            auto res = taskmgr_dump_tasks();
            const char *c = res.c_str();
            ESP_LOGI("TELE", "%s", c);
            send_rpc(IC("TSKS"), res.c_str(), res.length());

        }

        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}