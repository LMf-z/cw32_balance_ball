#include "Keys.h"

#include "cw32f003_gpio.h"
#include "cw32f003_rcc.h"

static void GPIO_init(uint32_t rcu, GPIO_TypeDef *GPIOx, uint32_t pin){
  CW_SYSCTRL->AHBEN |= rcu;
  GPIO_InitTypeDef init_struct;
  init_struct.Mode  = GPIO_MODE_INPUT_PULLUP;
  init_struct.Pins  = pin;
  init_struct.IT    = GPIO_IT_NONE;
  GPIO_Init(GPIOx, &init_struct);
}

void Keys_init(){
  __RCC_GPIOC_CLK_ENABLE();
  // Key按键
  GPIO_init(KEY1_RCU, KEY1_GPIO);
  GPIO_init(KEY2_RCU, KEY2_GPIO);
}

void Keys_scan(){
  static GPIO_PinState pre_state1 = GPIO_Pin_SET;
  static GPIO_PinState pre_state2 = GPIO_Pin_SET;
  
  GPIO_PinState state1 = GPIO_ReadPin(KEY1_GPIO);
  if(state1 != pre_state1){
    pre_state1 = state1;
    
    if(state1){ // 抬起
        Keys_on_key_up(1);
    }else {     // 按下
        Keys_on_key_down(1);
    }
  }
  
  
  GPIO_PinState state2 = GPIO_ReadPin(KEY2_GPIO);
  if(state2 != pre_state2){
    pre_state2 = state2;
    
    if(state2){ // 抬起
        Keys_on_key_up(2);
    }else {     // 按下
        Keys_on_key_down(2);
    }
  }
}
