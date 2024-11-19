#pragma once

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_encoder();
_Noreturn void encoder_run_demo(lv_obj_t **screens, int n);


#ifdef __cplusplus
}
#endif
