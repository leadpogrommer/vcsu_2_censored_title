#include <stdio.h>
#include "driver/i2c_master.h"
#include "esp_lcd_panel_ssd1306.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_io_interface.h"
#include <esp_lvgl_port.h>
#include <lvgl.h>

#include "display.h"


lv_display_t *disp = NULL;

void init_lcd(){
    i2c_master_bus_handle_t i2c_bus = NULL;
    i2c_master_bus_config_t bus_config = {
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .i2c_port = -1,
            .sda_io_num = 8,
            .scl_io_num = 9,
            .flags = {
                    .enable_internal_pullup = false,
            }
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&bus_config, &i2c_bus));

    esp_lcd_panel_io_handle_t io_handle = NULL;
    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = 0x3c,
        .control_phase_bytes = 1,
        .dc_bit_offset = 6,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .flags = {
            .dc_low_on_data = 0,
            .disable_control_phase = 0,
        },
        .scl_speed_hz = 400 * 1000,
    };
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {};
    panel_config.reset_gpio_num = -1;
    panel_config.bits_per_pixel = 1;

    esp_lcd_panel_ssd1306_config_t ssd1306_config = {
        .height = 64
    };
    panel_config.vendor_config = &ssd1306_config;

    esp_lcd_panel_handle_t panel_handle;

    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));


    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    esp_lcd_panel_disp_on_off(panel_handle, true);


    // lvgl stuff
    printf("Starting lvgl\n");
    const lvgl_port_display_cfg_t display_cfg = {
        .io_handle = io_handle,
        .panel_handle = panel_handle,
        .control_handle = 0,
        .buffer_size = 128 * 64,
        .double_buffer = true,
        .trans_size = 0,
        .hres = 128,
        .vres = 64,
        .monochrome = true,
        // .color_format = LV_COLOR_FORMAT_RGB565,
        .rotation = {
            .swap_xy = false,
            .mirror_x = 1,
            .mirror_y = 1,
        },
        .flags = {
            .buff_dma = 1,
            .buff_spiram = 0,
            .sw_rotate = 0,
//            .full_refresh = 1, // TODO: maybe remove this
            .direct_mode = 0,
        },
    };

    lvgl_port_cfg_t port_cfg = ESP_LVGL_PORT_INIT_CONFIG();
    port_cfg.task_max_sleep_ms = 30; // For some reason, lvgl (or _port) incorrectly calculates delay until next timer, so it's capped here to prevent render lag
    lvgl_port_init(&port_cfg);

    disp = lvgl_port_add_disp(&display_cfg);

    lvgl_port_lock(0);
    lv_theme_t *theme = lv_theme_mono_init(disp, false, lv_font_default());
    lv_disp_set_theme(disp, theme);

    lv_obj_t *label_ = lv_label_create(lv_disp_get_scr_act(disp));
    lv_label_set_text(label_, "Init");
    lv_obj_set_x(label_, 30);
    lv_obj_set_y(label_, 30);
    lvgl_port_unlock();
}