#pragma once



#ifdef __cplusplus
enum class TASK_KEY{
    LEFT, RIGHT, BUTTON
};

void taskmgr_handle_key(TASK_KEY key);

extern "C" {
#endif






void taskmgr_show_ui();
void taskmgr_init();
void taskmgr_run_js(const char* name, const char *code);

void taskmgr_update_progs_list(const char* buff, int len);

#ifdef __cplusplus
}
#endif
