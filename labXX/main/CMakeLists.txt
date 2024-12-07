idf_component_register(SRCS "bindings_io.cpp" "taskmgr.cpp" "bindings_gui.cpp" "bindings_timeout.cpp" "gui.cpp" "input.cpp" "display.c" "main.cpp" "network.c" "../thirdparty/duktape/prep/duk-esp-debug/duktape.c"
                    INCLUDE_DIRS "." "../thirdparty/duktape/prep/duk-esp-debug" REQUIRES esp_lcd esp_wifi nvs_flash esp32-rotary-encoder)
#target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++20)
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-label)