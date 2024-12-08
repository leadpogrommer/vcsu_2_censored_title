#pragma once



#ifdef __cplusplus
enum class TASK_KEY{
    LEFT, RIGHT, BUTTON
};

#include <string>

std::string taskmgr_dump_tasks();
void taskmgr_handle_key(TASK_KEY key);



extern "C" {
#endif




void taskmgr_handle_key_int(int key);

void taskmgr_show_ui();
void taskmgr_init();
void taskmgr_run_js(const char* name, const char *code);

void taskmgr_update_progs_list(const char* buff, int len);
void taskmgr_switc_task_id(int tid);
void taskmgr_kill_task_id(int tid);

#ifdef __cplusplus
}
#endif
