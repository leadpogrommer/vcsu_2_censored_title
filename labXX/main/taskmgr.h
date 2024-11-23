#pragma once


enum class TASK_KEY{
    LEFT, RIGHT, BUTTON
};

void taskmgr_handle_key(TASK_KEY key);
void taskmgr_show_ui();
void taskmgr_init();
void taskmgr_run_js(const char* name, const char *code);