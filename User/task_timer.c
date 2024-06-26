#include "task_timer.h"
#include "Task.h"

void Btim1_Init(void)
{
  BTIM_TimeBaseInitTypeDef BTIM_TimeBaseInitStruct;
  __RCC_BTIM_CLK_ENABLE();

  /* NVIC Configuration */
  NVIC_EnableIRQ(BTIM1_IRQn);

  BTIM_TimeBaseInitStruct.BTIM_Mode      = BTIM_Mode_TIMER;
  BTIM_TimeBaseInitStruct.BTIM_Period    = PCLK / 1000 - 1;      // 1ms²É¼¯1´Î
  BTIM_TimeBaseInitStruct.BTIM_Prescaler = BTIM_PRS_DIV1;        // 6MHZ / 1 / 1000
  BTIM_TimeBaseInitStruct.BTIM_OPMode    = BTIM_OPMode_Repetitive;

  BTIM_TimeBaseInit(CW_BTIM1, &BTIM_TimeBaseInitStruct);
  BTIM_ITConfig(CW_BTIM1, BTIM_IT_OV, ENABLE);
  BTIM_Cmd(CW_BTIM1, ENABLE);
}
void Task_timer_init(void){

  Btim1_Init();
  
}

void on_btim1_callback(void) {
  
  printf("TIM1\n");
  Task_switch_handler();
}
