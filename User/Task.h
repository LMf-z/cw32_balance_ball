#ifndef __TASK_H__
#define __TASK_H__

#include "cw32f003.h"
#include "App.h"

#define TASK_STATE_STOP       0 // 任务停止
#define TASK_STATE_RUN        1 // 任务可运行
#define TASK_STATE_SUSPEND    2 // 任务挂起(暂停)

// 定义函数指针类型
typedef void (*Task_callback)(void);

typedef struct {
  
  uint8_t     state;      // 任务状态
  uint16_t    count;      // 任务计数，倒计数到0，修改任务状态
  uint16_t   period;      // 任务周期，即多少ms执行一次（固定）
//  void (*callback)(void);
  Task_callback callback;        // 任务执行回调
  Task_callback callback_suspend; // 任务挂起回调
  
} Task_t;


// 任务可执行状态切换 timer 1ms
void Task_switch_handler(void);

// 任务执行的处理句柄
void Task_exec_handler(void);

// 根据任务函数，获取索引
int Task_get_task_id(Task_callback func_name);

// 根据任务索引，获取任务状态
uint8_t Task_get_task_state(uint8_t index);

// 暂停指定任务
void Task_suspend(uint8_t index);

// 恢复指定任务
void Task_resume(uint8_t index);


#endif