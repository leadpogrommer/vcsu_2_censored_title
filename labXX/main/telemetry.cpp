
#include "network.h"
#include "gui.h"
#include "taskmgr.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include <lvgl.h>
#include <esp_lvgl_port.h>


#include <string>

char img_buf[128*64];

void telemetry_task(){
    while(true){
        // send task list
        {
            auto res = taskmgr_dump_tasks();
            const char *c = res.c_str();
            send_rpc(IC("TSKS"), res.c_str(), res.length());

        }

        // send screen image
        {
            lv_img_dsc_t img;
            lvgl_port_lock(0);
            lv_snapshot_take_to_buf(lv_disp_get_scr_act(nullptr), LV_IMG_CF_ALPHA_1BIT, &img, img_buf, 128*64);
            // heap_caps_print_heap_info(MALLOC_CAP_DEFAULT);
            lvgl_port_unlock();

            // for(int i = 0; i < 128*64; i++){
            //     int bi = i / 8;
            //     int bpos = i % 8;
            //     img_buf[bi] |= (img_buf[i] != 0) << bpos;
            // }
            send_rpc(IC("SHOT"), img_buf, 128*64/8);
        }

        // send heap info
        {
            using std::to_string;
            multi_heap_info_t info;
            heap_caps_get_info(&info, MALLOC_CAP_DEFAULT);
            // TOtal free, largest block
            std::string res = to_string(info.total_free_bytes) + ";" + to_string(info.largest_free_block);
            send_rpc(IC("HEAP"), res.c_str(), res.length());
        }


        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}