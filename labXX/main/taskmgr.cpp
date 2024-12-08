#include "taskmgr.h"
#include "gui.h"
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"
#include "vector" // TODO: use some other container
#include <cstring>
#include <esp_log.h>

#include "network.h"

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

static void switch_task(gui_task_t *task, bool animate = true){
    lvgl_port_lock(0);
    active_group = task->button_group;
    if(animate){
        lv_scr_load_anim(task->screen, LV_SCR_LOAD_ANIM_MOVE_LEFT, 300, 0, false);
    } else {
        lv_scr_load(task->screen);
    }
    lvgl_port_unlock();
    is_edit_mode = false;
    lv_group_set_editing(active_group, is_edit_mode);
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

extern "C" void taskmgr_init(){
//    taskmgr_lock = xSemaphoreCreateMutex();

    lvgl_port_lock(0);
    init_taskmgr_ui();
    init_run_ui();
    lvgl_port_unlock();

    add_task(&run_menu_ui_task);
    switch_task(&run_menu_ui_task);


}

// TODO: do this better
static uint32_t lv_kl = LV_KEY_DOWN;
static uint32_t lv_kr = LV_KEY_UP;


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
            lv_group_set_editing(active_group, is_edit_mode);
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
//            lv_group_send_data(active_group, key == TASK_KEY::LEFT ? LV_KEY_LEFT : LV_KEY_RIGHT);
            if(focused_obj) lv_event_send(focused_obj, LV_EVENT_KEY, key == TASK_KEY::LEFT ? &lv_kl : &lv_kr);
        }
    }
    TM_U();
}
extern "C" void taskmgr_handle_key_int(int key){
    taskmgr_handle_key((TASK_KEY)key);
}

extern "C" void taskmgr_show_ui(){
    TM_L();
    if(active_group != taskmgr_ui_task.button_group){
        switch_task(&taskmgr_ui_task);
    }
    TM_U();
}

extern "C" void taskmgr_run_js(const char* name, const char *code){
    gui_task_t *task = run_js_task(code);
    TM_L();
    strncpy(task->name, name, 9);
    task->name[9] = 0;
    task->is_js = true;
    add_task(task);
    switch_task(task);
    TM_U();
}


static void ui_run_prog_button_handler(lv_event_t *e){
    TM_L();
    // TODO: this code sucks, it blocks render task
    char* text = static_cast<char *>(e->user_data);
    send_rpc(IC("RUN "), text, strlen(text));
    TM_U();
}



extern "C" void taskmgr_update_progs_list(const char* buff, int len){
    TM_L();

    for (int i = 0; i < lv_obj_get_child_cnt(run_menu_ui_task.screen); i++){
        lv_obj_del_async(lv_obj_get_child(run_menu_ui_task.screen, i));
    }

    auto prog_list = lv_list_create(run_menu_ui_task.screen);
    lv_group_add_obj(run_menu_ui_task.button_group, prog_list);
    lv_obj_set_x(prog_list, 0);
    lv_obj_set_y(prog_list, 0);
    lv_obj_set_height(prog_list, 64);
    lv_obj_set_width(prog_list, 128);

    int pos = 0;
    while (pos < len){
        int l = strlen(buff+pos);
        auto btn = lv_list_add_btn(prog_list, LV_SYMBOL_PLAY, buff + pos);
        pos = pos + l + 1;

        lv_obj_add_event_cb(btn, ui_run_prog_button_handler, LV_EVENT_CLICKED, (void *)lv_list_get_btn_text(prog_list, btn));
        lv_group_add_obj(run_menu_ui_task.button_group, btn);
        lv_obj_set_style_outline_width(btn, 1, LV_STATE_FOCUSED);
        lv_obj_set_style_outline_pad(btn, 1, LV_STATE_FOCUSED);
        lv_obj_set_style_pad_all(btn, 1, 0);
        lv_obj_set_style_pad_gap(btn, 1, 0);
    }


    TM_U();
}

std::string taskmgr_dump_tasks(){
    TM_L();

    std::string res;
    for(auto task: tasks){
        if(!task->is_js) continue;
        res += std::to_string(task->tid);
        res += ';';
        res += task->name;
        res += '/';
    }

    TM_U();

    return res;
}

void taskmgr_switc_task_id(int tid){
    TM_L();
    for(auto task: tasks){
       if(task->tid == tid){
        switch_task(task);
        break;
       }
    }
    TM_U();
}

void taskmgr_kill_task_id(int tid){
    TM_L();

    int to_del = -1;
    for(int i = 0; i < tasks.size(); i++){
       if(tasks[i]->tid == tid){
        to_del = i;
        break;
       }
    }


    if(to_del != -1){

        for(int i = 0; i < lv_obj_get_child_cnt(ui_task_list); i++){
            auto btn = lv_obj_get_child(ui_task_list, i);
            if(lv_obj_get_event_user_data(btn, ui_task_button_handler) == tasks[to_del]){
                lv_obj_del(btn);
                break;
            }
        }

        switch_task(&run_menu_ui_task, false);
        kill_task(tasks[to_del]);
        tasks.erase(tasks.begin() + to_del);
    }

    TM_U();
}
