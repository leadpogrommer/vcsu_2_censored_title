#include "gui.h"

#include <esp_log.h>
#include <driver/gpio.h>
#include <unordered_set>

static const std::unordered_set<char> pins = {0, 1, 2, 3, 4, 10};

// 0 - output
DUK_BI(lp_bi_pin_mode){
    auto mode = duk_require_int(ctx, -1);
    auto pin = (gpio_num_t)duk_require_int(ctx, -2);
    if(!pins.contains(pin)){
        ESP_LOGE(ct()->name, "pinMode: Incorrect pin: %d", pin);
        return DUK_RET_RANGE_ERROR;
    }
    if(mode != 0 /*&& mode != 1*/){
        ESP_LOGE(ct()->name, "pinMode: Incorrect pin mode: %d", mode);
        return DUK_RET_RANGE_ERROR;
    }

    // assume mode 0
    gpio_reset_pin(pin);
    gpio_set_pull_mode(pin, GPIO_FLOATING);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);

    return 0;
}

DUK_BI(lp_bi_digital_write){
    auto val = duk_require_boolean(ctx, -1);
    auto pin = (gpio_num_t) duk_require_int(ctx, -2);
    if(!pins.contains(pin)){
        ESP_LOGE(ct()->name, "digitalWrite: Incorrect pin: %d", pin);
        return DUK_RET_RANGE_ERROR;
    }

    gpio_set_level(pin, val);

    return 0;
}