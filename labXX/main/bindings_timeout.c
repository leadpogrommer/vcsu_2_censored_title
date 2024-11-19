#include "gui.h"
#include "esp_log.h"

#define TAG "JS_TIMEOUT"


// TODO: this does not actually set timeout
duk_ret_t lp_bi_set_timeout(duk_context *ctx){
    ESP_LOGI(TAG, "Set timeout called");
    duk_double_t delay = duk_require_number(ctx, -1);
    duk_require_callable(ctx, -2);

    // DEBUG
    duk_dup(ctx, -2);
    duk_call(ctx, 0);

    return 0;
}

duk_ret_t lp_bi_print(duk_context *ctx){
    printf("[JSLOG] ");
    for(int i = 0; i < duk_get_top(ctx); i++){
        printf("%s ", duk_to_string(ctx, i));
    }
    printf("\n");
    return 0;
}