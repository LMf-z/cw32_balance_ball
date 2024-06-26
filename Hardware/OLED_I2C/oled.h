#ifndef __OLED_H
#define __OLED_H 

#include "cw32f003.h"
#include "stdlib.h"	
#include "I2C.h"

#ifndef u8
#define u8 uint8_t
#endif

#ifndef u16
#define u16 uint16_t
#endif

#ifndef u32
#define u32 uint32_t
#endif

#define I2C_WRITE(addr, reg, dat, len)            I2C_write(addr, reg, dat, len)
#define I2C_READ(addr, reg, dat, len)             I2C_read(addr, reg, dat, len)


#define OLED_CMD  0	//д����
#define OLED_DATA 1	//д����

void OLED_ClearPoint(u8 x,u8 y);
//void I2C_Start(void);
//void I2C_Stop(void);
//void I2C_WaitAck(void);
//void Send_Byte(u8 dat);
void OLED_WR_Byte(u8 dat,u8 mode);
void OLED_Refresh(void);
void OLED_Clear(u8 is_refresh);
void OLED_DrawPoint(u8 x,u8 y,u8 t);
void OLED_DrawLine(u8 x1,u8 y1,u8 x2,u8 y2,u8 mode);
void OLED_DrawRectangle(u8 x1,u8 y1,u8 w,u8 h,u8 is_fill, u8 mode);
void OLED_ShowChar(u8 x,u8 y,char chr,u8 size1,u8 mode);
void OLED_ShowString(u8 x,u8 y,char *chr,u8 size1,u8 mode);
void OLED_Init(void);

#if 0
void OLED_ColorTurn(u8 i);
void OLED_DisplayTurn(u8 i);
void OLED_DisPlay_On(void);
void OLED_DisPlay_Off(void);
void OLED_DrawCircle(u8 x,u8 y,u8 r);
void OLED_ShowChar6x8(u8 x,u8 y,u8 chr,u8 mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode);
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode);
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode);
#endif


#endif

