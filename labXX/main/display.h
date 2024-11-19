#pragma once

#include <lvgl.h>
#include "esp_lvgl_port.h"

#ifdef __cplusplus
extern "C" {
#endif

extern lv_display_t *disp;
void init_lcd();

#ifdef __cplusplus
}
#endif
