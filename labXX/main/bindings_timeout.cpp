#include "gui.h"
#include "esp_log.h"

#define TAG "JS_TIMEOUT"


// TODO: this does not actually set timeout
DUK_BI(lp_bi_set_timeout){
    ESP_LOGI(TAG, "Set timeout called");
    duk_double_t delay = duk_require_number(ctx, -1);
    duk_require_callable(ctx, -2);

    // DEBUG
    duk_dup(ctx, -2);
    duk_call(ctx, 0);

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