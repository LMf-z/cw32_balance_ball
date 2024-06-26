#include "Bluetooth.h"
#include "cw32f003_rcc.h"
#include "cw32f003_gpio.h"
#include "cw32f003_uart.h"


/**********************************************************
 @brief UART2初始化

 波特率: 115200
 RX: PA7
 TX: PA6

 **********************************************************/
 
#define BAUDRATE  115200UL
 
static void UART2_init() {
  // GPIO -----------------------------------------------------
  RCC_AHBPeriphClk_Enable(RCC_AHB_PERIPH_GPIOA, ENABLE);      //使能串口要用到的GPIO时钟

  GPIO_InitTypeDef GPIO_InitStructure;                        //GPIO初始化
  
  // RX
  GPIO_InitStructure.IT = GPIO_IT_NONE;
  GPIO_InitStructure.Pins =  GPIO_PIN_7;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT_PULLUP;
  GPIO_Init(CW_GPIOA, &GPIO_InitStructure);
  PA07_AFx_UART2RXD();

  // TX
  GPIO_InitStructure.IT = GPIO_IT_NONE;
  GPIO_InitStructure.Pins = GPIO_PIN_6;
  GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_Init(CW_GPIOA, &GPIO_InitStructure);
  PA06_AFx_UART2TXD();

  // UART2 -----------------------------------------------------
  RCC_APBPeriphClk_Enable1(RCC_APB1_PERIPH_UART2, ENABLE);    //使能串口时钟

  USART_InitTypeDef USART_InitStructure;                       //串口初始化
  USART_InitStructure.USART_BaudRate = BAUDRATE;                 //设置波特率
  USART_InitStructure.USART_Over = USART_Over_16;              //串口采样方式
  USART_InitStructure.USART_Source = USART_Source_PCLK;        //串口传输时钟源采用PCLK
  USART_InitStructure.USART_UclkFreq = RCC_Sysctrl_GetPClkFreq();  //频率为PCLK的频率
  USART_InitStructure.USART_StartBit = USART_StartBit_FE;      //数据开始位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;       //数据停止位
  USART_InitStructure.USART_Parity = USART_Parity_No ;         //无校验
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //发送/接收使能
  USART_Init(CW_UART2, &USART_InitStructure);
}

void Bluetooth_init() {
  UART2_init();
}

void Bluetooth_send_string(char* str) {

  while(str && *str != '\0'){  // \0表示数据结束
    USART_SendData_8bit(CW_UART2, *str);    // 发送一个字节(8bit)
    while (USART_GetFlagStatus(CW_UART2, USART_FLAG_TXE) == RESET);  //发送缓冲器未空则等待
    str++;              //发送一次完成后，准备发送下一位数据
  }
  while (USART_GetFlagStatus(CW_UART2, USART_FLAG_TXBUSY) == SET);     //发送串口状态忙则等待  

}

void Bluetooth_send_data(uint8_t* data, uint32_t len) {
  while(data && len--){
    USART_SendData_8bit(CW_UART2, *data);
    while (USART_GetFlagStatus(CW_UART2, USART_FLAG_TXE) == RESET);  //发送缓冲器未空则等待
    data++;
  }
  while (USART_GetFlagStatus(CW_UART2, USART_FLAG_TXBUSY) == SET);     //发送串口状态忙则等待  
}