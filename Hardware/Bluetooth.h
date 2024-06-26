#ifndef __BLUETOOTH_H__
#define __BLUETOOTH_H__

#include "cw32f003.h"

void Bluetooth_init();

void Bluetooth_send_string(char* data);

void Bluetooth_send_data(uint8_t* data, uint32_t len);



#endif