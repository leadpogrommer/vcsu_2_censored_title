idf_component_register(SRCS "input.c" "display.c" "main.c" "network.c"
                    INCLUDE_DIRS "." REQUIRES esp_lcd esp_wifi nvs_flash esp32-rotary-encoder)
#target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++20)