#ifndef __KEYS_H__
#define __KEYS_H__

#include "cw32f003.h"

// PC03
#define KEY1_RCU    RCC_AHB_PERIPH_GPIOC
#define KEY1_GPIO   CW_GPIOC, GPIO_PIN_3

// PC02
#define KEY2_RCU    RCC_AHB_PERIPH_GPIOC
#define KEY2_GPIO   CW_GPIOC, GPIO_PIN_2


void Keys_init();

void Keys_scan();

extern void Keys_on_key_down(uint8_t btn);
extern void Keys_on_key_up(uint8_t btn);

#endif