#ifndef __EC11_H__
#define __EC11_H__

#include "cw32f003.h"

void EC12_init(void);

/**
 * @brief EC12旋钮编码器旋转事件
 * 
 * @param dir 0: 逆时针, 1: 顺时针
 */
extern void EC12_on_changed(uint8_t dir);

#endif 