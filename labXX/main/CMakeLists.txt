idf_component_register(SRCS "display.c" "labXX.c" "network.c"
                    INCLUDE_DIRS "." REQUIRES esp_lcd esp_wifi nvs_flash)
#target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++20)