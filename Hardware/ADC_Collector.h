#ifndef __ADC_COLLECTOR_H__
#define __ADC_COLLECTOR_H__

#include "cw32f003.h"

// ��¼һЩӲ������
#define ADC_VREF  (1500) // mV
#define R2        (220)  // K��
#define R1        (10)   // K��
//#define R2        (216)  // K��
//#define R1        (9.78)   // K��

#define ADC_SAMPLE_SIZE   20

extern uint16_t Volt_ADC_Value;      //�洢ADC����ֵ

extern uint16_t Volt_ADC_Values[];      //�洢�����ѹADC����ֵ

extern uint16_t Curr_ADC_Values[];      //�洢�������ADC����ֵ

void ADC_Collector_init(void);

void ADC_Collector_get(void);

float ADC_Collector_get_voltage(void);
float ADC_Collector_get_current(void);


#endif