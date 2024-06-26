#include "EC12.h"
#include "cw32f003_gpio.h"
#include "cw32f003_rcc.h"

void EC12_init(void){

      // GPIO --------------  PB07  PB00

  __RCC_GPIOB_CLK_ENABLE();

  GPIO_InitTypeDef GPIO_InitStruct;
  GPIO_InitStruct.IT = GPIO_IT_RISING | GPIO_IT_FALLING;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
  GPIO_InitStruct.Pins = GPIO_PIN_7;
  GPIO_Init(CW_GPIOB, &GPIO_InitStruct);    // EC_A PB07 BTIMETR

  GPIO_InitStruct.IT = GPIO_IT_NONE;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT_PULLUP;
  GPIO_InitStruct.Pins = GPIO_PIN_0;
  GPIO_Init(CW_GPIOB, &GPIO_InitStruct);    // EC_B PB00

  //配置中断滤波
  GPIO_ConfigFilter(CW_GPIOB, bv7, GPIO_FLTCLK_RC10K);

  //清除PB05、PB06中断标志并使能NVIC
  GPIOB_INTFLAG_CLR(bv7);

  __disable_irq();
  NVIC_EnableIRQ(GPIOB_IRQn);
  __enable_irq();

}

void GPIOB_IRQHandleCallback(void)
{

  if (CW_GPIOB->ISR_f.PIN7)    // 中断中，直接操作寄存器，速度快
  {
    CW_GPIOB->ICR_f.PIN7 = 0;    // 清标志

    uint8_t value0 = PB00_GETVALUE();
    if(value0 == 0) {
      return;
    }

    uint8_t value7 = PB07_GETVALUE();

    if(value7 == 0) {
      EC12_on_changed(1);
    } else {
      EC12_on_changed(0);
    }

//    Servo_set_angle(180 - ((Cnt + 90) * 2));
  }
}