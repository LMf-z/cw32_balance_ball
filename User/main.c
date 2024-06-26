#include "cw32f003.h"
#include "App.h"

#include "UART1.h"
#include "I2C.h"
#include "Task.h"
 #include "task_timer.h"

/***********************************************
���� ��װ����������

--------------------------------------------

��������񿪷����v1.0

- ͨ��Timer��ʱ�����δ�ʱ������ÿ��ִ��1000�� 1ms

��������񿪷����v1.5

- ����ָ��������ָ�����׼ȷִ��
- ����ͬ��������ൽ��ͬ���ļ�(Application)
  1. App_Input.c      �����¼���������
  2. App_Balance.c    ��ȡ���룬���ƶ������
  3. App_Protocol.c   Э��ͨѶ(�շ���Ϣ)����
  4. App_OLED.c       ��Ļ��ʾ����


��������񿪷����v2.0

************************************************/

void RCC_Configuration(void)
{
  FLASH_SetLatency(FLASH_Latency_2);        // ������ƵΪ48MHZ��Ҫע�⣬Flah�ķ���������Ҫ����ΪFLASH_Latency_2��
  RCC_HSI_Enable(RCC_HSIOSC_DIV1);          // ����Ƶ��Ϊ48M
  RCC_SYSCLKSRC_Config(RCC_SYSCLKSRC_HSI);  // ѡ��SYSCLKʱ��Դ  48MHz

  RCC_HCLKPRS_Config(RCC_HCLK_DIV1);        // ����SYSTICK��HCLK��Ƶϵ��  48MHz
  RCC_PCLKPRS_Config(RCC_PCLK_DIV8);        // ����HCLK �� PCLK�ķ�Ƶϵ��  6MHz

  InitTick(HCLK);                           // SYSTICK �Ĺ���Ƶ��Ϊ48MHz��ÿms�ж�һ��
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
