idf_component_register(SRCS "bindings_gui.c" "bindings_timeout.c" "gui.c" "input.c" "display.c" "main.c" "network.c" "../../duktape_test/duktape/prep/duk-esp-debug/duktape.c"
                    INCLUDE_DIRS "." "../../duktape_test/duktape/prep/duk-esp-debug" REQUIRES esp_lcd esp_wifi nvs_flash esp32-rotary-encoder)
#target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++20)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-label)