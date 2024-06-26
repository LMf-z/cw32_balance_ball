#include "cw32f003.h"
#include "App.h"

#include "UART1.h"
#include "I2C.h"
#include "Task.h"
 #include "task_timer.h"

/***********************************************
任务： 封装裸机开发框架

--------------------------------------------

裸机多任务开发框架v1.0

- 通过Timer定时器（滴答定时器），每秒执行1000次 1ms

裸机多任务开发框架v1.5

- 能让指定任务按照指定间隔准确执行
- 将不同的任务分类到不同的文件(Application)
  1. App_Input.c      按键事件处理任务
  2. App_Balance.c    获取距离，控制舵机任务
  3. App_Protocol.c   协议通讯(收发消息)任务
  4. App_OLED.c       屏幕显示任务


裸机多任务开发框架v2.0

************************************************/

void RCC_Configuration(void)
{
  FLASH_SetLatency(FLASH_Latency_2);        // 设置主频为48MHZ需要注意，Flah的访问周期需要更改为FLASH_Latency_2。
  RCC_HSI_Enable(RCC_HSIOSC_DIV1);          // 设置频率为48M
  RCC_SYSCLKSRC_Config(RCC_SYSCLKSRC_HSI);  // 选择SYSCLK时钟源  48MHz

  RCC_HCLKPRS_Config(RCC_HCLK_DIV1);        // 配置SYSTICK到HCLK分频系数  48MHz
  RCC_PCLKPRS_Config(RCC_PCLK_DIV8);        // 配置HCLK 到 PCLK的分频系数  6MHz

  InitTick(HCLK);                           // SYSTICK 的工作频率为48MHz，每ms中断一次
}

void On_SysTick_Update(void){
  Task_switch_handler();
}

int32_t main(void)
{
  RCC_Configuration();

  UART1_init();
  I2C_init();

  printf("Angle\n");

  Task_timer_init();
  
#if !LASER_PID_SWITCH
  App_ADC_init(); 
#endif
  App_Data_Transfer_init();
  App_PID_Control_init();
  App_OLED_Menu_init();
  while(1)
  {
    Task_exec_handler();
  }

}
