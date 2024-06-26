#ifndef __SEGMENT_H__
#define __SEGMENT_H__

#include "cw32f003_gpio.h"
#include "cw32f003_rcc.h"

void Segment_Init(void);

void Segment_Show(uint8_t Pos,uint8_t Num);

void Segment_Display(float Vrst);
void Segment_DisplayI(float Irst);

void Segment_Close_Com(void);

void Segment_Refresh(void);

#endif