#include "oled.h"

#include <math.h>

#include "oledfont.h"
#include "stdlib.h"

// 144行 8列
// 128 x 64

// u8 OLED_GRAM[144][8];
// u8 OLED_GRAM[8][144];
u8 OLED_GRAM[8][128];

#define DEV_ADDR 0x3C

#define SCREEN_W 128
#define SCREEN_H 64

// 发送一个字节
// mode:数据/命令标志 0,表示命令;1,表示数据;
void OLED_WR_Byte(u8 dat, u8 mode) {
  if (mode) {
    // 数据
    I2C_WRITE(DEV_ADDR, 0x40, &dat, 1);
  } else {
    // 命令
    I2C_WRITE(DEV_ADDR, 0x00, &dat, 1);
  }
}
// ------------------------------------------------- OLED业务驱动
#if 0

//反显函数
void OLED_ColorTurn(u8 i)
{
  if(i==0)
  {
    OLED_WR_Byte(0xA6,OLED_CMD);//正常显示
  }
  if(i==1)
  {
    OLED_WR_Byte(0xA7,OLED_CMD);//反色显示
  }
}

//屏幕旋转180度
void OLED_DisplayTurn(u8 i)
{
  if(i==0)
  {
    OLED_WR_Byte(0xC8,OLED_CMD);//正常显示
    OLED_WR_Byte(0xA1,OLED_CMD);
  }
  if(i==1)
  {
    OLED_WR_Byte(0xC0,OLED_CMD);//反转显示
    OLED_WR_Byte(0xA0,OLED_CMD);
  }
}
// 开启OLED显示
void OLED_DisPlay_On(void)
{
  OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
  OLED_WR_Byte(0x14,OLED_CMD);//开启电荷泵
  OLED_WR_Byte(0xAF,OLED_CMD);//点亮屏幕
}

//关闭OLED显示
void OLED_DisPlay_Off(void)
{
  OLED_WR_Byte(0x8D,OLED_CMD);//电荷泵使能
  OLED_WR_Byte(0x10,OLED_CMD);//关闭电荷泵
  OLED_WR_Byte(0xAE,OLED_CMD);//关闭屏幕
}
#endif

// 更新显存到OLED
void OLED_Refresh(void) {
  u8 i, n;
  for (i = 0; i < 8; i++)  // 外循环遍历行
  {
    OLED_WR_Byte(0xb0 + i, OLED_CMD);  // 设置行起始地址
    OLED_WR_Byte(0x00, OLED_CMD);      // 设置低列起始地址
    OLED_WR_Byte(0x10, OLED_CMD);      // 设置高列起始地址
    // 把指定列的所有数据128字节，全部发送出去
    I2C_WRITE(DEV_ADDR, 0x40, &OLED_GRAM[i][0], 128);
  }
}
// 清屏函数
void OLED_Clear(u8 is_refresh) {
  u8 i, n;
  for (i = 0; i < 8; i++) {
    for (n = 0; n < 128; n++) {
      OLED_GRAM[i][n] = 0x00;  // 清除所有数据
    }
  }
  if (is_refresh) {
    // 更新显示
    OLED_Refresh();
  }
}

// 画点
// x:0~127
// y:0~63
// t:1 填充turn on  0,清空turn off
void OLED_DrawPoint(u8 x, u8 y, u8 t) {
  u8 i = y / 8;  // 行数
  u8 n = 1 << (y % 8);
  if (t) {  // 把指定行列的oled灯 点亮, 设置1
    OLED_GRAM[i][x] |= n;
  } else {  // 把指定行列的oled灯 熄灭, 设置0
    OLED_GRAM[i][x] &= ~n;
  }
}

// 设置单步方向和计算坐标增量
//void setDirectionAndDelta(int delta, int *inc, int *delta_val) {
//  if (delta > 0)
//    *inc = 1;
//  else if (delta == 0)
//    *inc = 0;
//  else {
//    *inc = -1;
//    *delta_val = -*delta_val;
//  }
//}
// 画线
// x1,y1:起点坐标
// x2,y2:结束坐标
void OLED_DrawLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode) {
  u16 t;
  int xerr=0,yerr=0,delta_x,delta_y,distance;
  int incx,incy;
  int uRow=x1, uCol=y1; //画线起点坐标
  delta_x=x2-x1; //计算坐标增量
  delta_y=y2-y1;
  if(delta_x>0)incx=1; //设置单步方向
  else if (delta_x==0)incx=0;//垂直线
  else {
    incx=-1;
    delta_x=-delta_x;
  }
  if(delta_y>0)incy=1;
  else if (delta_y==0)incy=0;//水平线
  else {
    incy=-1;
    delta_y=-delta_x;
  }
  if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴
  else distance=delta_y;
  for(t=0; t<distance+1; t++)
  {
    OLED_DrawPoint(uRow,uCol,mode);//画点
    xerr+=delta_x;
    yerr+=delta_y;
    if(xerr>distance)
    {
      xerr-=distance;
      uRow+=incx;
    }
    if(yerr>distance)
    {
      yerr-=distance;
      uCol+=incy;
    }
  }
//  u16 t;
//  int xerr = 0, yerr = 0, delta_x, delta_y, distance;
//  int incx, incy;
//  int uRow = x1, uCol = y1;  // 画线起点坐标
//  delta_x = x2 - x1;         // 计算坐标增量
//  delta_y = y2 - y1;
//  setDirectionAndDelta(delta_x, &incx, &delta_x);
//  setDirectionAndDelta(delta_y, &incy, &delta_y);

//  // 选取基本增量坐标轴
//  distance = (delta_x > delta_y) ? delta_x : delta_y;

//  for (t = 0; t <= distance; t++) {
//    OLED_DrawPoint(uRow, uCol, mode);  // 画点
//    xerr += delta_x;
//    yerr += delta_y;
//    if (xerr > distance) {
//      xerr -= distance;
//      uRow += incx;
//    }
//    if (yerr > distance) {
//      yerr -= distance;
//      uCol += incy;
//    }
//  }
}

void OLED_DrawRectangle(u8 x1, u8 y1, u8 w, u8 h, u8 is_fill, u8 mode) {
  OLED_DrawLine(x1, y1, x1 + w, y1, mode);
  OLED_DrawLine(x1 + w, y1, x1 + w, y1 + h, mode);
  OLED_DrawLine(x1, y1, x1, y1 + h, mode);
  OLED_DrawLine(x1, y1 + h, x1 + w, y1 + h, mode);

  if (!is_fill) {
    return;
  }

  for (int i = 1; i < h; i++) {
    OLED_DrawLine(x1, y1 + i, x1 + w, y1 + i, mode);
  }
}

// 在指定位置显示一个字符,包括部分字符
// x:0~127
// y:0~63
// size1:选择字体 6x8/6x12/8x16/12x24
// mode:0,反色显示;1,正常显示
void OLED_ShowChar(u8 x, u8 y, char chr, u8 size1, u8 mode) {
  u8 i, m, temp, size2, chr1;
  u8 x0 = x, y0 = y;
  if (size1 == 8)
    size2 = 6;
  else
    size2 = (size1 / 8 + ((size1 % 8) ? 1 : 0)) *
            (size1 / 2);  // 得到字体一个字符对应点阵集所占的字节数
  chr1 = chr - ' ';  // 计算偏移后的值
  for (i = 0; i < size2; i++) {
    if (size1 == 8) {
      temp = asc2_0806[chr1][i];  // 调用0806字体
    }
    // else if(size1==12)
    // {
    //   temp=asc2_1206[chr1][i]; //调用1206字体
    // }
    //    else if(size1==16)
    //    {
    //      temp=asc2_1608[chr1][i]; //调用1608字体
    //    }
    //    else if(size1==24)
    //    {
    //      temp=asc2_2412[chr1][i]; //调用2412字体
    //    }
    else
      return;
    for (m = 0; m < 8; m++) {
      if (temp & 0x01)
        OLED_DrawPoint(x, y, mode);
      else
        OLED_DrawPoint(x, y, !mode);
      temp >>= 1;
      y++;
    }
    x++;
    if ((size1 != 8) && ((x - x0) == size1 / 2)) {
      x = x0;
      y0 = y0 + 8;
    }
    y = y0;
  }
}

// 显示字符串
// x,y:起点坐标
// size1:字体大小
//*chr:字符串起始地址
// mode:0,反色显示;1,正常显示
void OLED_ShowString(u8 x, u8 y, char *chr, u8 size1, u8 mode) {
  while ((*chr >= ' ') && (*chr <= '~'))  // 判断是不是非法字符!
  {
    OLED_ShowChar(x, y, *chr, size1, mode);
    if (size1 == 8)
      x += 6;
    else
      x += size1 / 2;
    chr++;
  }
}

// m^n
u32 OLED_Pow(u8 m, u8 n) {
  u32 result = 1;
  while (n--) {
    result *= m;
  }
  return result;
}

#if 0

//x,y:圆心坐标
//r:圆的半径
void OLED_DrawCircle(u8 x,u8 y,u8 r)
{
  int a, b,num;
  a = 0;
  b = r;
  while(2 * b * b >= r * r)
  {

    OLED_DrawPoint(x + a, y - b,1);
    OLED_DrawPoint(x - a, y - b,1);
    OLED_DrawPoint(x - a, y + b,1);
    OLED_DrawPoint(x + a, y + b,1);

    OLED_DrawPoint(x + b, y + a,1);
    OLED_DrawPoint(x + b, y - a,1);
    OLED_DrawPoint(x - b, y - a,1);
    OLED_DrawPoint(x - b, y + a,1);

    a++;
    num = (a * a + b * b) - r*r;//计算画的点离圆心的距离
    if(num > 0)
    {
      b--;
      a--;
    }
  }
}
//显示数字
//x,y :起点坐标
//num :要显示的数字
//len :数字的位数
//size:字体大小
//mode:0,反色显示;1,正常显示
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode)
{
  u8 t,temp,m=0;
  if(size1==8)m=2;
  for(t=0; t<len; t++)
  {
    temp=(num/OLED_Pow(10,len-t-1))%10;
    if(temp==0)
    {
      OLED_ShowChar(x+(size1/2+m)*t,y,'0',size1,mode);
    }
    else
    {
      OLED_ShowChar(x+(size1/2+m)*t,y,temp+'0',size1,mode);
    }
  }
}

//显示汉字
//x,y:起点坐标
//num:汉字对应的序号
//mode:0,反色显示;1,正常显示
void OLED_ShowChinese(u8 x,u8 y,u8 num,u8 size1,u8 mode)
{
  u8 m,temp;
  u8 x0=x,y0=y;
  u16 i,size3=(size1/8+((size1%8)?1:0))*size1;  //得到字体一个字符对应点阵集所占的字节数
  for(i=0; i<size3; i++)
  {
    if(size1==16)
    {
      temp=Hzk1[num][i]; //调用16*16字体
    }
//    else if(size1==24)
//    {
//      temp=Hzk2[num][i]; //调用24*24字体
//    }
//    else if(size1==32)
//    {
//      temp=Hzk3[num][i]; //调用32*32字体
//    }
//    else if(size1==64)
//    {
//      temp=Hzk4[num][i]; //调用64*64字体
//    }
    else return;
    for(m=0; m<8; m++)
    {
      if(temp&0x01)OLED_DrawPoint(x,y,mode);
      else OLED_DrawPoint(x,y,!mode);
      temp>>=1;
      y++;
    }
    x++;
    if((x-x0)==size1)
    {
      x=x0;
      y0=y0+8;
    }
    y=y0;
  }
}

//num 显示汉字的个数
//space 每一遍显示的间隔
//mode:0,反色显示;1,正常显示
void OLED_ScrollDisplay(u8 num,u8 space,u8 mode)
{
  u8 i,n,t=0,m=0,r;
  while(1)
  {
    if(m==0)
    {
      OLED_ShowChinese(128,24,t,16,mode); //写入一个汉字保存在OLED_GRAM[][]数组中
      t++;
    }
    if(t==num)
    {
      for(r=0; r<16*space; r++)    //显示间隔
      {
        for(i=1; i<144; i++)
        {
          for(n=0; n<8; n++)
          {
            OLED_GRAM[n][i-1]=OLED_GRAM[n][i];
          }
        }
        OLED_Refresh();
      }
      t=0;
    }
    m++;
    if(m==16) {
      m=0;
    }
    for(i=1; i<144; i++) //实现左移
    {
      for(n=0; n<8; n++)
      {
        OLED_GRAM[n][i-1]=OLED_GRAM[n][i];
      }
    }
    OLED_Refresh();
  }
}

//x,y：起点坐标
//sizex,sizey,图片长宽
//BMP[]：要写入的图片数组
//mode:0,反色显示;1,正常显示
void OLED_ShowPicture(u8 x,u8 y,u8 sizex,u8 sizey,u8 BMP[],u8 mode)
{
  u16 j=0;
  u8 i,n,temp,m;
  u8 x0=x,y0=y;
  sizey=sizey/8+((sizey%8)?1:0);
  for(n=0; n<sizey; n++)
  {
    for(i=0; i<sizex; i++)
    {
      temp=BMP[j];
      j++;
      for(m=0; m<8; m++)
      {
        if(temp&0x01)OLED_DrawPoint(x,y,mode);
        else OLED_DrawPoint(x,y,!mode);
        temp>>=1;
        y++;
      }
      x++;
      if((x-x0)==sizex)
      {
        x=x0;
        y0=y0+8;
      }
      y=y0;
    }
  }
}

#endif

// OLED的初始化
void OLED_Init(void) {
  //	OLED_RES_Clr();
  FirmwareDelay(200);
  //	OLED_RES_Set();

  OLED_WR_Byte(0xAE, OLED_CMD);  //--turn off oled panel
  OLED_WR_Byte(0x00, OLED_CMD);  //---set low column address
  OLED_WR_Byte(0x10, OLED_CMD);  //---set high column address
  OLED_WR_Byte(0x40, OLED_CMD);  //--set start line address  Set Mapping RAM
  // Display Start Line (0x00~0x3F)
  OLED_WR_Byte(0x81, OLED_CMD);  //--set contrast control register
  OLED_WR_Byte(0xCF, OLED_CMD);  // Set SEG Output Current Brightness
  OLED_WR_Byte(0xA1, OLED_CMD);  //--Set SEG/Column Mapping     0xa0左右反置 0xa1正常
  OLED_WR_Byte(0xC8, OLED_CMD);  // Set COM/Row Scan Direction   0xc0上下反置 0xc8正常
  OLED_WR_Byte(0xA6, OLED_CMD);  //--set normal display
  OLED_WR_Byte(0xA8, OLED_CMD);  //--set multiplex ratio(1 to 64)
  OLED_WR_Byte(0x3f, OLED_CMD);  //--1/64 duty
  OLED_WR_Byte(0xD3, OLED_CMD);  //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
  OLED_WR_Byte(0x00, OLED_CMD);  //-not offset
  OLED_WR_Byte(0xd5, OLED_CMD);  //--set display clock divide ratio/oscillator frequency
  OLED_WR_Byte(0x80, OLED_CMD);  //--set divide ratio, Set Clock as 100 Frames/Sec
  OLED_WR_Byte(0xD9, OLED_CMD);  //--set pre-charge period
  OLED_WR_Byte(0xF1, OLED_CMD);  // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
  OLED_WR_Byte(0xDA, OLED_CMD);  //--set com pins hardware configuration
  OLED_WR_Byte(0x12, OLED_CMD);
  OLED_WR_Byte(0xDB, OLED_CMD);  //--set vcomh
  OLED_WR_Byte(0x40, OLED_CMD);  // Set VCOM Deselect Level
  OLED_WR_Byte(0x20, OLED_CMD);  //-Set Page Addressing Mode (0x00/0x01/0x02)
  OLED_WR_Byte(0x02, OLED_CMD);  //
  OLED_WR_Byte(0x8D, OLED_CMD);  //--set Charge Pump enable/disable
  OLED_WR_Byte(0x14, OLED_CMD);  //--set(0x10) disable
  OLED_WR_Byte(0xA4, OLED_CMD);  // Disable Entire Display On (0xa4/0xa5)
  OLED_WR_Byte(0xA6, OLED_CMD);  // Disable Inverse Display On (0xa6/a7)

  OLED_Clear(1);
  OLED_WR_Byte(0xAF, OLED_CMD);
}
