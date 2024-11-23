#include <sys/cdefs.h>
#include "freertos/FreeRTOS.h"
#include "rotary_encoder.h"
#include <esp_lvgl_port.h>
#include <lvgl.h>
#include "esp_log.h"
#include <gui.h>
#include "taskmgr.h"


#include "input.h"
QueueHandle_t encoder_raw_event_queue;
rotary_encoder_info_t encoder_info = {};

static auto btn_pin = static_cast<gpio_num_t>(5);
static void init_encoder(){
    // setup encoder
    ESP_ERROR_CHECK(gpio_install_isr_service(0));


    ESP_ERROR_CHECK(rotary_encoder_init(&encoder_info, static_cast<gpio_num_t>(7), static_cast<gpio_num_t>(6)));
    ESP_ERROR_CHECK(rotary_encoder_enable_half_steps(&encoder_info, false));

    encoder_raw_event_queue = rotary_encoder_create_queue();
    ESP_ERROR_CHECK(rotary_encoder_set_queue(&encoder_info, encoder_raw_event_queue));
}



static void init_button(){

    gpio_reset_pin(btn_pin);
    gpio_set_pull_mode(btn_pin, GPIO_PULLUP_ONLY);
    gpio_set_direction(btn_pin, GPIO_MODE_INPUT);
    gpio_set_intr_type(btn_pin, GPIO_INTR_ANYEDGE);
}

#define TAG "imgr"


[[noreturn]] static void input_manager_task(void *arg){
    ESP_LOGI(TAG, "Input manager running...");
    int64_t prev_button_change_micros = 0;
    bool is_button_pressed = false;
    bool taskmgr_was_opened = false;

    rotary_encoder_event_t enc_ev;
    while (true){
        if(xQueueReceive(encoder_raw_event_queue, &enc_ev, 30/portTICK_PERIOD_MS) == pdTRUE && enc_ev.state.direction != ROTARY_ENCODER_DIRECTION_NOT_SET){
            // we got something from encoder
            ESP_LOGI(TAG, "Encoder moved, pos = %ld, button state = %d", enc_ev.state.position, is_button_pressed);
            if(is_button_pressed){
                taskmgr_was_opened = true;
                taskmgr_show_ui();
            }else{
                taskmgr_handle_key(enc_ev.state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? TASK_KEY::RIGHT : TASK_KEY::LEFT);
            }
        }
        auto button_pin_state = !(bool)gpio_get_level(btn_pin);
        auto current_time_micros = esp_timer_get_time();
        if((current_time_micros > prev_button_change_micros + 100000) && (button_pin_state != is_button_pressed)){
            prev_button_change_micros = current_time_micros;
            is_button_pressed = button_pin_state;
            ESP_LOGI(TAG, "Button state changed to %d", is_button_pressed);
            if(!is_button_pressed ){
                if (!taskmgr_was_opened){
                    taskmgr_handle_key(TASK_KEY::BUTTON);
                } else {
                    taskmgr_was_opened = false;
                }
            }
        }
    }
}

void start_input_manager(){
    init_encoder();
    init_button();
    xTaskCreate(input_manager_task, "Inpt", 3000, nullptr, 10, nullptr);
}
//void encoder_run_demo(gui_task_t **tasks, int n){
//    int cscr = 0;
//
//    lvgl_port_lock(0);
//    lv_scr_load_anim(tasks[0]->screen, LV_SCR_LOAD_ANIM_MOVE_RIGHT , 500, 0, false);
//    lvgl_port_unlock();
//
//
//    ESP_LOGI(TAG, "Encoder demo loop starting...");
//    while (1) {
//        rotary_encoder_event_t event = { 0 };
//        if (xQueueReceive(encoder_raw_event_queue, &event, portMAX_DELAY) == pdTRUE) {
//            ESP_LOGI(TAG, "Event: position %ld, direction %s", event.state.position,
//                     event.state.direction ? (event.state.direction == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? "CW" : "CCW") : "NOT_SET");
//            rotary_encoder_direction_t dir = event.state.direction;
////            if (dir == ROTARY_ENCODER_DIRECTION_NOT_SET) continue;
////            cscr += dir == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? 1 : -1;
////            if(cscr < 0) cscr = n-1;
////            cscr %= n;
//            lvgl_port_lock(0);
//
//            lv_group_t *g = tasks[cscr]->button_group;
//            if(dir == ROTARY_ENCODER_DIRECTION_CLOCKWISE){
//                lv_group_focus_next(g);
//            } else {
//                lv_group_focus_prev(g); // TODO: this sets wrong focus flag
//            }
//            lv_group_send_data(tasks[cscr]->button_group, dir == ROTARY_ENCODER_DIRECTION_CLOCKWISE ? LV_KEY_RIGHT : LV_KEY_LEFT);
//            lv_obj_t* focused = lv_group_get_focused(tasks[cscr]->button_group);
//            printf("Focused button: %s\n", lv_label_get_text(lv_obj_get_child(focused, 0)));
////            lv_scr_load_anim(tasks[cscr].screen, dir != ROTARY_ENCODER_DIRECTION_CLOCKWISE ? LV_SCR_LOAD_ANIM_MOVE_RIGHT : LV_SCR_LOAD_ANIM_MOVE_LEFT, 500, 0, false);
//            lvgl_port_unlock();
//
//
//        } else {
//            ESP_LOGE(TAG, "fuck?");
//        }
//    }
//}