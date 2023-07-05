#ifndef RTOS_COMMON
#define RTOS_COMMON

void create_stop_all_task(int time_limit_ms = 5000, int tasks_left = -1);
void finish_task();


#endif