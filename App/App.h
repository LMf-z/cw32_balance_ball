#ifndef __APP_H__
#define __APP_H__

#include "cw32f003.h"
#include "base_types.h"
#include "system_cw32f003.h"
#include "cw32f003_gpio.h"
#include "cw32f003_rcc.h"
#include "cw32f003_systick.h"
#include "cw32f003_flash.h"
#include "cw32f003_btim.h"
#include "cw32f003_gtim.h"

#define HCLK  48000000UL
#define PCLK  6000000UL
#define PI    3.14159265358979323846

#define CLIP_VALUE(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

#define LASER_PID_SWITCH  1

/*************************** App_OLED_Menu ************************/

#define NODE_SERVO    10
#define NODE_LASER    20
#define NODE_PID      30
#define NODE_ADC      40

extern volatile int16_t menu_level;

void App_OLED_Menu_init();

void App_OLED_Menu_keys();
void App_OLED_Menu_screen();

/*************************** App_PID_Control ************************/
extern int current_mm;
extern float kp;
extern float kd;
extern float ki;
void App_PID_Control_init();
void App_PID_Control_task();

/*************************** App_ADC ************************/
extern float Vrst;
extern float Irst;
void App_ADC_init();
void App_ADC_task();

/*************************** App_Data_Transfer ************************/
void App_Data_Transfer_init();
void App_Data_Transfer_task();

#endif