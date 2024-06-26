#ifndef __SERVO_H__
#define __SERVO_H__

#include "cw32f003.h"

void Servo_init(void);

void Servo_set_angle(float angle);

float Servo_get_angle(void);

#endif