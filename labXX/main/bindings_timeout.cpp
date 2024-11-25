#include "gui.h"
#include "esp_log.h"
#include <esp_timer.h>

#define TAG "JS_TIMEOUT"


//typedef struct {
//    gui_task_t *task;
//    int cb_id;
//    esp_timer_handle_t esp_timer;
//}callback_data_t;






class TimerCallback: public Callback {
public:
    explicit TimerCallback(gui_task_t *task): Callback(task) {
        esp_timer = 0;
    }

    void afterRunHook() override {
        delete this;
    }

    ~TimerCallback() override {
        esp_timer_stop(esp_timer);
        esp_timer_delete(esp_timer);
    }

public:
    esp_timer_handle_t esp_timer;

};


void timer_cb(Callback *arg){
    xQueueSend(arg->task->event_queue, &arg, portMAX_DELAY);
}


// TODO: this does not actually set timeout
DUK_BI(lp_bi_set_timeout){
//    ESP_LOGI(TAG, "Set timeout called");
    duk_double_t delay = duk_require_number(ctx, -1);
    duk_require_callable(ctx, -2);
    // [ ... callback duration ]

    duk_pop(ctx);
    // [... callback ]

    auto* cb = new TimerCallback(ct());


    esp_timer_create_args_t timer_args = {
            .callback = reinterpret_cast<esp_timer_cb_t>(timer_cb),
            .arg = cb,
            .dispatch_method = ESP_TIMER_TASK,
            .name = "JS Timer",
            .skip_unhandled_events = false
    };

    esp_timer_create(&timer_args, &cb->esp_timer);
    esp_timer_start_once(cb->esp_timer, delay * 1000);

    return 0;
}

DUK_BI(lp_bi_print){
    printf("[JSLOG] ");
    for(int i = 0; i < duk_get_top(ctx); i++){
        printf("%s ", duk_to_string(ctx, i));
    }
    printf("\n");
    return 0;
}