#include "App.h"
#include "ADC_Collector.h"

void on_btim2_callback(void) {
  static uint32_t Cnt=0;

// printf("ADC: %d\n", Cnt);

  // ÿ1msִ��һ�β���
  ADC_Collector_get();

  Cnt++;
  if (Cnt >= 3) { //3ms��ʾһ������� 1000ms / (6�� * 3ms) = 55.5֡
    Cnt = 0;
//    Segment_Refresh(); //�����ɨ����ʾ
  }
}

void Btim2_Init(void)
{
  BTIM_TimeBaseInitTypeDef BTIM_TimeBaseInitStruct;
  __RCC_BTIM_CLK_ENABLE();

  /* NVIC Configuration */
  NVIC_EnableIRQ(BTIM2_IRQn);

  BTIM_TimeBaseInitStruct.BTIM_Mode      = BTIM_Mode_TIMER;
  BTIM_TimeBaseInitStruct.BTIM_Period    = PCLK / 1000 - 1;      // 1ms�ɼ�1��
  BTIM_TimeBaseInitStruct.BTIM_Prescaler = BTIM_PRS_DIV1;        // 6MHZ / 1 / 1000
  BTIM_TimeBaseInitStruct.BTIM_OPMode    = BTIM_OPMode_Repetitive;

  BTIM_TimeBaseInit(CW_BTIM2, &BTIM_TimeBaseInitStruct);
  BTIM_ITConfig(CW_BTIM2, BTIM_IT_OV, ENABLE);
  BTIM_Cmd(CW_BTIM2, ENABLE);
}

void App_ADC_init(void) {
  Btim2_Init();
  // ��ʼ��ADC�ɼ���
  ADC_Collector_init();
}

float Vrst = 0;
float Irst = 0;
void App_ADC_task(void) {
    // ��ȡת����ĵ�ѹֵ, ��λ: mV
    Vrst = ADC_Collector_get_voltage();

    // ��ȡת����ĵ���ֵ, ��λ: mA
    Irst = ADC_Collector_get_current();
    
    // ׼������
    // printf("vol=%.2f cur=%.2f\n", Vrst, Irst);
}