#include "led.h"

// LED��״̬: 0��, 1��
extern uint8_t g_led_state;

void LED_init(){

  // GPIO ---------------------------
  
  // rcu
  __RCC_GPIOB_CLK_ENABLE();
  __RCC_GPIOC_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct;
  //set PB00 / PB01 / PB02 / PB03 as output
  
  // PB7 - COM1 λѡ
  GPIO_InitStruct.Pins = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.IT   = GPIO_IT_NONE;
  GPIO_Init(CW_GPIOB, &GPIO_InitStruct);

  // PC4 - A λѡ
  GPIO_InitStruct.Pins = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.IT   = GPIO_IT_NONE;
  GPIO_Init(CW_GPIOC, &GPIO_InitStruct);

  //set PB05 / PB06 as inupt with pur / int = none
//    GPIO_InitStruct.Pins = GPIO_PIN_5 | GPIO_PIN_6;
//    GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
//    GPIO_InitStruct.IT   = GPIO_IT_NONE;
//    GPIO_Init( CW_GPIOB , &GPIO_InitStruct);

  // ���� PB7(COM1) , ���� PC4 (A)�ɵ��� �����1��a
  GPIO_WritePin( CW_GPIOB, GPIO_PIN_7, GPIO_Pin_RESET);
  
  GPIO_WritePin( CW_GPIOC, GPIO_PIN_4, GPIO_Pin_RESET);

}

void LED_loop(){
  
  GPIO_WritePin( CW_GPIOC, GPIO_PIN_4, g_led_state ? GPIO_Pin_SET : GPIO_Pin_RESET);
}