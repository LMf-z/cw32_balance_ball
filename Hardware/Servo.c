#include "Servo.h"

#include "cw32f003_gpio.h"
#include "cw32f003_rcc.h"
#include "cw32f003_systick.h"
#include "cw32f003_gtim.h"


#define PCLK      6000000UL
#define FREQ    50    // 频率
#define PERIOD  (PCLK / 128 / FREQ)

static void GPIO_Configuration(void)
{
  /* PB2 作为GTIM的CH3 PWM 输出 */
  __RCC_GPIOB_CLK_ENABLE();

  PB02_AFx_GTIMCH3();
  PB02_DIGTAL_ENABLE();
  PB02_DIR_OUTPUT();
  PB02_PUSHPULL_ENABLE();

}

void Servo_motor_config() {
  GTIM_InitTypeDef GTIM_InitStruct;

  __RCC_GTIM_CLK_ENABLE();

  GTIM_InitStruct.Mode = GTIM_MODE_TIME; // GTIM_MODE_COUNTER, GTIM_MODE_TIME
  GTIM_InitStruct.OneShotMode = GTIM_COUNT_CONTINUE;
  GTIM_InitStruct.Prescaler = GTIM_PRESCALER_DIV128;    // DCLK = PCLK / 2 = 6MHz/2 = 3MHz
  GTIM_InitStruct.ReloadValue = PERIOD - 1;     // Max: 65535
  GTIM_InitStruct.ToggleOutState = DISABLE;

  GTIM_TimeBaseInit(&GTIM_InitStruct);
  GTIM_OCInit(GTIM_CHANNEL3, GTIM_OC_OUTPUT_PWM_LOW);
  GTIM_SetCompare3(0);
//  GTIM_ITConfig(GTIM_IT_OV, ENABLE);
  GTIM_ITConfig(GTIM_IT_OV, DISABLE);

  GTIM_Cmd(ENABLE);
}

// GTIM_CH3
void Servo_init(void) {

  GPIO_Configuration();
  Servo_motor_config();

}

#define  FIX_RATIO  0.95
float current_angle = 90;
void Servo_set_angle(float angle) {
  // [0, 180] -> [500, 2500] / 20000

  if (angle > 180) angle = 180.0;
  else if (angle < 0) angle = 0;
  current_angle = angle;
  float percent = (500 + (angle * FIX_RATIO / 180) * 2000) / 20000;

  GTIM_SetCompare3(PERIOD * percent - 1);

}

float Servo_get_angle() {
  return current_angle;
}