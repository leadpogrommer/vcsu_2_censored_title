#include <sys/cdefs.h>
#include <esp_lvgl_port.h>
#include <lvgl.h>

extern lv_display_t *disp;

void connect_to_wifi();
void init_lcd();

void init_encoder();

_Noreturn void encoder_run_demo(lv_obj_t **screens, int n);