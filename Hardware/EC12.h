#ifndef __EC11_H__
#define __EC11_H__

#include "cw32f003.h"

void EC12_init(void);

/**
 * @brief EC12��ť��������ת�¼�
 * 
 * @param dir 0: ��ʱ��, 1: ˳ʱ��
 */
extern void EC12_on_changed(uint8_t dir);

#endif 