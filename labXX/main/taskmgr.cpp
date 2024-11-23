#include "taskmgr.h"
#include "gui.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "vector" // TODO: use some other container
#include <cstring>
#include <esp_log.h>

//#define TM_L() xSemaphoreTake(taskmgr_lock, portMAX_DELAY)
//#define TM_U() xSemaphoreGive(taskmgr_lock)

#define TM_L() lvgl_port_lock(0)
#define TM_U() lvgl_port_unlock();

//SemaphoreHandle_t taskmgr_lock;
#define TAG "taskmgr"

static gui_task_t taskmgr_ui_task, run_menu_ui_task;
static std::vector<gui_task_t *> tasks = {};

static lv_group_t *active_group;
static int current_task_index = 0;

static bool is_edit_mode = false;

lv_obj_t *ui_task_list;

static void init_taskmgr_ui(){
    taskmgr_ui_task.is_js = false;
    taskmgr_ui_task.screen = lv_obj_create(nullptr);
    taskmgr_ui_task.button_group = lv_group_create();
    auto scr = taskmgr_ui_task.screen;

    auto lbl = lv_label_create(scr);
    lv_label_set_text(lbl, "Tasks:");

    // TODO: fix this
    auto lbl_h = lv_obj_get_height(lbl);
    ui_task_list = lv_list_create(scr);
    lv_obj_set_x(ui_task_list, 0);
    lv_obj_set_y(ui_task_list, lbl_h);
    lv_obj_set_width(ui_task_list, 128);
    lv_obj_set_height(ui_task_list, 64 - lbl_h);
//    lv_group_add_obj(taskmgr_ui_task.button_group, ui_task_list);


}

static void init_run_ui(){
    run_menu_ui_task.is_js = false;
    run_menu_ui_task.screen = lv_obj_create(nullptr);
    run_menu_ui_task.button_group = lv_group_create();

    auto* lbl = lv_label_create(run_menu_ui_task.screen);
    lv_label_set_text(lbl, "Task runner\nwill be\nhere");
    strcpy(run_menu_ui_task.name, "Run");
}

static void switch_task(gui_task_t *task){
    lvgl_port_lock(0);
    active_group = task->button_group;
    lv_scr_load_anim(task->screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    lvgl_port_unlock();
    is_edit_mode = false;
}

static void ui_task_button_handler(lv_event_t *e){
    TM_L();
    auto *selected_task = static_cast<gui_task_t *>(lv_event_get_user_data(e));
    switch_task(selected_task);
    TM_U();
}



static void add_task(gui_task_t *t){
    tasks.push_back(t);
    auto btn = lv_list_add_btn(ui_task_list, LV_SYMBOL_BULLET, t->name);
    lv_obj_add_event_cb(btn, ui_task_button_handler, LV_EVENT_CLICKED, t);
    lv_group_add_obj(taskmgr_ui_task.button_group, btn);
    lv_obj_set_style_outline_width(btn, 1, LV_STATE_FOCUSED);
    lv_obj_set_style_outline_pad(btn, 1, LV_STATE_FOCUSED);
    lv_obj_set_style_pad_all(btn, 1, 0);
    lv_obj_set_style_pad_gap(btn, 1, 0);
}

void taskmgr_init(){
//    taskmgr_lock = xSemaphoreCreateMutex();

    lvgl_port_lock(0);
    init_taskmgr_ui();
    init_run_ui();
    lvgl_port_unlock();

    add_task(&run_menu_ui_task);
    switch_task(&run_menu_ui_task);


}


// EVERYTHING BELOW THIS POINT SHOULD TAKE THE MUTEX
void taskmgr_handle_key(TASK_KEY key){
    TM_L();
    auto focused_obj = lv_group_get_focused(active_group);
    if(key == TASK_KEY::BUTTON){
        bool should_change_mode = false;
        if(focused_obj){
            should_change_mode = lv_obj_is_editable(focused_obj);
        }
        if(should_change_mode){
            is_edit_mode = !is_edit_mode;
        } else{
            ESP_LOGI(TAG, "sending keypress");
//            lv_group_send_data(active_group, LV_KEY_ENTER);
            if(focused_obj) lv_event_send(focused_obj, LV_EVENT_CLICKED, nullptr);
        }
    } else if (key == TASK_KEY::LEFT || key == TASK_KEY::RIGHT){
        if(!is_edit_mode){
            if(key == TASK_KEY::LEFT) lv_group_focus_prev(active_group);
            else lv_group_focus_next(active_group);
        } else {
            lv_group_send_data(active_group, key == TASK_KEY::LEFT ? LV_KEY_LEFT : LV_KEY_RIGHT);
        }
    }
    TM_U();
}
void taskmgr_show_ui(){
    TM_L();
    if(active_group != taskmgr_ui_task.button_group){
        switch_task(&taskmgr_ui_task);
    }
    TM_U();
}

void taskmgr_run_js(const char* name, const char *code){
    gui_task_t *task = run_js_task(code);
    TM_L();
    strncpy(task->name, name, 9);
    add_task(task);
    TM_U();
}