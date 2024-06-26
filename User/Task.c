#include "Task.h"

static Task_t task_list[] = {
   // state,         count, period,  callback

  { TASK_STATE_RUN,   20,    20,  App_OLED_Menu_keys },
  { TASK_STATE_RUN,  100,   100,  App_OLED_Menu_screen },
  { TASK_STATE_RUN,  200,   200,  App_Data_Transfer_task },
#if !LASER_PID_SWITCH
  { TASK_STATE_RUN,  200,   200,  App_ADC_task },
#else
  { TASK_STATE_RUN,   20,    20,  App_PID_Control_task },
#endif
};

uint8_t task_cnt = sizeof(task_list) / sizeof(Task_t);

// 任务可执行状态切换 timer 1ms执行一次
void Task_switch_handler(void){
  for(uint8_t i = 0; i < task_cnt; i++){
    // 判断如果任务是挂起状态，跳过切换判定
    if (task_list[i].state == TASK_STATE_SUSPEND){
      continue;
    }
    
    task_list[i].count--;
    if (task_list[i].count == 0){
      // 任务状态state切换
      task_list[i].state = TASK_STATE_RUN;
      // 重置count计数值
      task_list[i].count = task_list[i].period;
    }
  }
}

// 任务执行的处理句柄
void Task_exec_handler(void){
  // 将状态为 TASK_STATE_RUN 运行起来，标记恢复成STOP
  for (uint8_t i = 0; i < task_cnt; i++){
    if (task_list[i].state == TASK_STATE_RUN){
      task_list[i].callback();
      task_list[i].state = TASK_STATE_STOP;
    }
  }
}

// 根据任务函数名，查找对应位置
int Task_get_task_id(Task_callback func_name){
  for (uint8_t i = 0; i < task_cnt; i++){
    if(task_list[i].callback == func_name){
      return i;
    }
  }
  return -1;
}

// 根据任务索引，获取任务状态
uint8_t Task_get_task_state(uint8_t index){
  if(index >= task_cnt) return TASK_STATE_STOP;
  return task_list[index].state;
}

// 暂停指定任务
void Task_suspend(uint8_t index){
  if(index >= task_cnt) return;
  
  task_list[index].state = TASK_STATE_SUSPEND;
  
  // 挂起回调函数不是NULL，才执行
  if (task_list[index].callback_suspend != NULL){
    task_list[index].callback_suspend();
  }
}

// 恢复指定任务
void Task_resume(uint8_t index){
  if(index >= task_cnt) return;
  
  // 重置count计数值
  task_list[index].count = task_list[index].period;
  task_list[index].state = TASK_STATE_RUN;
  
}