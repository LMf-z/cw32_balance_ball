#ifndef __ADC_COLLECTOR_H__
#define __ADC_COLLECTOR_H__

#include "cw32f003.h"

// 记录一些硬件参数
#define ADC_VREF  (1500) // mV
#define R2        (220)  // KΩ
#define R1        (10)   // KΩ
//#define R2        (216)  // KΩ
//#define R1        (9.78)   // KΩ

#define ADC_SAMPLE_SIZE   20

extern uint16_t Volt_ADC_Value;      //存储ADC采样值

extern uint16_t Volt_ADC_Values[];      //存储多个电压ADC采样值

extern uint16_t Curr_ADC_Values[];      //存储多个电流ADC采样值

void ADC_Collector_init(void);

void ADC_Collector_get(void);

float ADC_Collector_get_voltage(void);
float ADC_Collector_get_current(void);


#endif