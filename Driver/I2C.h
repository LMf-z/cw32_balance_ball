#ifndef __I2C_H__
#define __I2C_H__

#include "cw32f003.h"

#include "cw32f003_gpio.h"
#include "cw32f003_rcc.h"
#include "cw32f003_i2c.h"
#include "cw32f003_systick.h"

#define I2C_SOFT   1   // 0硬实现，1软实现


// PB01, PB06, PB04, PA05(SWCLK)
// PB04
#define I2C_SCL_RCU     RCC_AHB_PERIPH_GPIOB
#define I2C_SCL_PORT    CW_GPIOB
#define I2C_SCL_PIN     GPIO_PIN_4

// PB00, PA02(SWDIO), PB05, PB03, PA06
// PB03
#define I2C_SDA_RCU     RCC_AHB_PERIPH_GPIOB
#define I2C_SDA_PORT    CW_GPIOB
#define I2C_SDA_PIN     GPIO_PIN_3

#define I2C_SDA_OUT()   PB03_DIR_OUTPUT();
#define I2C_SDA_IN()    PB03_DIR_INPUT();

#define I2C_SDA_AF()    PB03_AFx_I2CSDA();
#define I2C_SCL_AF()    PB04_AFx_I2CSCL();

#define SDA(bit)  GPIO_WritePin(I2C_SDA_PORT, I2C_SDA_PIN, (bit) ? GPIO_Pin_SET : GPIO_Pin_RESET)
#define SCL(bit)  GPIO_WritePin(I2C_SCL_PORT, I2C_SCL_PIN, (bit) ? GPIO_Pin_SET : GPIO_Pin_RESET)

// 读取SDA电平
#define I2C_SDA_STATE() GPIO_ReadPin(I2C_SDA_PORT, I2C_SDA_PIN)

#define I2C_SPEED       100000UL
//#define I2C_SPEED       400000UL

#define delay_us(cnt)   FirmwareDelay(cnt * 10)
#define delay_ms(cnt)   SysTickDelay(cnt)

void I2C_init();

/* 写数据到I2C设备指定寄存器 */
uint8_t I2C_write(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t len);

/* 从I2C设备指定寄存器读取数据 */
uint8_t I2C_read(uint8_t addr, uint8_t reg, uint8_t* dat, uint32_t len);


#endif