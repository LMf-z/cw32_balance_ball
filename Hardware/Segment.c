#include "Segment.h"

/*  共阴数码管编码表：
 0x3f   0x06   0x5b  0x4f  0x66  0x6d  0x7d  0x07  0x7f  0x6f 
  0      1      2     3     4     5     6     7     8     9 
 0xbf   0x86   0xdb  0xcf  0xe6  0xed  0xfd  0x87  0xff  0xef           
  0.     1.     2.    3.    4.    5.    6.    7.    8.    9.          */
 
             
uint8_t Seg_Table[20] = {0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07, 0x7f, 0x6f,
                         0xbf, 0x86, 0xdb, 0xcf, 0xe6, 0xed, 0xfd, 0x87, 0xff, 0xef};

uint8_t Seg_Reg[6] = {0};

void Segment_Init() {

  __RCC_GPIOA_CLK_ENABLE();//打开GPIOA的时钟
  __RCC_GPIOB_CLK_ENABLE();//打开GPIOB的时钟
  __RCC_GPIOC_CLK_ENABLE();//打开GPIOC的时钟

	GPIO_InitTypeDef GPIO_InitStruct; 

	GPIO_InitStruct.Pins = GPIO_PIN_0 | GPIO_PIN_4|GPIO_PIN_1; //PA00,E;PA04,G
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.IT = GPIO_IT_NONE;
  GPIO_Init(CW_GPIOA, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pins = GPIO_PIN_6 | GPIO_PIN_4 | GPIO_PIN_2 | GPIO_PIN_0 | GPIO_PIN_3 | GPIO_PIN_7; //PB06,B;PB04,C;PB02,D;PB00,F;PB03,DP  //PB07，COM1
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.IT = GPIO_IT_NONE;
  GPIO_Init(CW_GPIOB, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pins = GPIO_PIN_4 | GPIO_PIN_3 | GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0; //PC04,A; //PC03,COM2;PC02,COM3
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.IT = GPIO_IT_NONE;
  GPIO_Init(CW_GPIOC, &GPIO_InitStruct);
}

// Segment 段 (纵向Y坐标)
#define SEG1  CW_GPIOC, GPIO_PIN_4
#define SEG2  CW_GPIOB, GPIO_PIN_6
#define SEG3  CW_GPIOB, GPIO_PIN_4
#define SEG4  CW_GPIOB, GPIO_PIN_2
#define SEG5  CW_GPIOA, GPIO_PIN_0
#define SEG6  CW_GPIOB, GPIO_PIN_0
#define SEG7  CW_GPIOA, GPIO_PIN_4
#define SEG8  CW_GPIOB, GPIO_PIN_3

// Common 公共端 (横向X坐标)
#define COM1  CW_GPIOB, GPIO_PIN_7
#define COM2  CW_GPIOC, GPIO_PIN_3
#define COM3  CW_GPIOC, GPIO_PIN_2
#define COM4  CW_GPIOA, GPIO_PIN_1
#define COM5  CW_GPIOC, GPIO_PIN_0
#define COM6  CW_GPIOC, GPIO_PIN_1

void Segment_Show(uint8_t Pos,uint8_t Num) {

	Segment_Close_Com();//先关闭所有公共端,防止重影
  
  uint8_t Dis_Value = Seg_Table[Num];

  GPIO_WritePin(SEG1,(Dis_Value >> 0) & 0x01); //PC04,A
  GPIO_WritePin(SEG2,(Dis_Value >> 1) & 0x01); //PB06,B
  GPIO_WritePin(SEG3,(Dis_Value >> 2) & 0x01); //PB04,C
  GPIO_WritePin(SEG4,(Dis_Value >> 3) & 0x01); //PB02,D
  GPIO_WritePin(SEG5,(Dis_Value >> 4) & 0x01); //PA00,E
  GPIO_WritePin(SEG6,(Dis_Value >> 5) & 0x01); //PB00,F
  GPIO_WritePin(SEG7,(Dis_Value >> 6) & 0x01); //PA04,G
  GPIO_WritePin(SEG8,(Dis_Value >> 7) & 0x01); //PB03,DP

  switch(Pos) {
    case 0:
      GPIO_WritePin(COM1,GPIO_Pin_RESET);  
      break;
    case 1:
      GPIO_WritePin(COM2,GPIO_Pin_RESET);  
      break;
    case 2:
      GPIO_WritePin(COM3,GPIO_Pin_RESET);  
      break;
    case 3:
      GPIO_WritePin(COM4,GPIO_Pin_RESET);  
      break;
    case 4:
      GPIO_WritePin(COM5,GPIO_Pin_RESET);  
      break;
    case 5:
      GPIO_WritePin(COM6,GPIO_Pin_RESET);  
      break;
    default:
      break;
  }
}

void Segment_Display_old(uint16_t value)
{
  uint8_t Hundreds; // 百位数
  uint8_t Tens;     // 十位数
  uint8_t Units;    // 个位数
	
	Units     = value % 10;
	Tens      = value / 10 % 10;
  Hundreds  = value / 100 % 10;
	
	Seg_Reg[0] = Hundreds;
	Seg_Reg[1] = Tens;
	Seg_Reg[2] = Units;
}

uint16_t round_to_u16(float value){
  return (uint16_t) (value + 0.5f);
}

/**********************************************************
 * @brief 显示电压值
 * @param 被测电压值, 单位mV

 Vref最大 1500mV
 Vrst范围 0mV -> 34500mV
          0V  -> 34.5V
 
 **********************************************************/
void Segment_Display(float Vrst) // mV
{
  
  uint8_t num0; // 百位数
  uint8_t num1; // 十位数
  uint8_t num2; // 个位数
	
  uint16_t value = round_to_u16(Vrst * 0.1); // [0, 3450]
  
  if(value < 1000){   
    // 电压在10V以内， 显示x.xx伏. eg. 562 -> 5.62
    num0  = value / 100 % 10;
    num1  = value / 10 % 10;
    num2  = value % 10;
      
    Seg_Reg[0] = num0 + 10; // 加dp显示
    Seg_Reg[1] = num1;
    Seg_Reg[2] = num2;
  
  }else { 
    // 电压在10V及以上， 显示xx.x伏 . eg.2356 -> 23.6
    if (value % 10 > 5) value += 10;
    
    num0  = value / 1000 % 10;
    num1  = value / 100 % 10;
    num2  = value / 10 % 10;
    
    Seg_Reg[0] = num0;
    Seg_Reg[1] = num1 + 10; // 加dp显示
    Seg_Reg[2] = num2;
  }
  
}
/**********************************************************
 * @brief 显示电流值
 * @param 被测电流值, 单位mA

 Irst范围 0mV -> 3000mV

把最高位放最前边: Irst / 10
 
 **********************************************************/
void Segment_DisplayI(float Irst) // mA
{
  uint16_t value = round_to_u16(Irst * 0.1); // [0, 300]
  
  // 电流在3A以内， 显示x.xx A
  Seg_Reg[3] =  value / 100 + 10;          // 加dp显示
  Seg_Reg[4] =  value / 10 % 10;
  Seg_Reg[5] =  value % 10;
  
}

/**
 * @brief 关闭所有公共端
 * 
 */
void Segment_Close_Com(){
  GPIO_WritePin(COM1,GPIO_Pin_SET);
  GPIO_WritePin(COM2,GPIO_Pin_SET);
  GPIO_WritePin(COM3,GPIO_Pin_SET);
  GPIO_WritePin(COM4,GPIO_Pin_SET);
  GPIO_WritePin(COM5,GPIO_Pin_SET);
  GPIO_WritePin(COM6,GPIO_Pin_SET);
}


/**
 * @brief 数码管扫描显示函数,定时器周期性调用
 *
 */
void Segment_Refresh(void)
{
  static uint8_t pos = 0;

	Segment_Show(pos, Seg_Reg[pos]);
  
  // 下一次调用, 跳到下一个数字
	pos++;
  pos %= 6;
}