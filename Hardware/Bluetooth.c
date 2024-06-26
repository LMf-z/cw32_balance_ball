#include "Bluetooth.h"
#include "cw32f003_rcc.h"
#include "cw32f003_gpio.h"
#include "cw32f003_uart.h"


/**********************************************************
 @brief UART2��ʼ��

 ������: 115200
 RX: PA7
 TX: PA6

 **********************************************************/
 
#define BAUDRATE  115200UL
 
static void UART2_init() {
  // GPIO -----------------------------------------------------
  RCC_AHBPeriphClk_Enable(RCC_AHB_PERIPH_GPIOA, ENABLE);      //ʹ�ܴ���Ҫ�õ���GPIOʱ��

  GPIO_InitTypeDef GPIO_InitStructure;                        //GPIO��ʼ��
  
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
  RCC_APBPeriphClk_Enable1(RCC_APB1_PERIPH_UART2, ENABLE);    //ʹ�ܴ���ʱ��

  USART_InitTypeDef USART_InitStructure;                       //���ڳ�ʼ��
  USART_InitStructure.USART_BaudRate = BAUDRATE;                 //���ò�����
  USART_InitStructure.USART_Over = USART_Over_16;              //���ڲ�����ʽ
  USART_InitStructure.USART_Source = USART_Source_PCLK;        //���ڴ���ʱ��Դ����PCLK
  USART_InitStructure.USART_UclkFreq = RCC_Sysctrl_GetPClkFreq();  //Ƶ��ΪPCLK��Ƶ��
  USART_InitStructure.USART_StartBit = USART_StartBit_FE;      //���ݿ�ʼλ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;       //����ֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No ;         //��У��
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //����/����ʹ��
  USART_Init(CW_UART2, &USART_InitStructure);
}

void Bluetooth_init() {
  UART2_init();
}

void Bluetooth_send_string(char* str) {

  while(str && *str != '\0'){  // \0��ʾ���ݽ���
    USART_SendData_8bit(CW_UART2, *str);    // ����һ���ֽ�(8bit)
    while (USART_GetFlagStatus(CW_UART2, USART_FLAG_TXE) == RESET);  //���ͻ�����δ����ȴ�
    str++;              //����һ����ɺ�׼��������һλ����
  }
  while (USART_GetFlagStatus(CW_UART2, USART_FLAG_TXBUSY) == SET);     //���ʹ���״̬æ��ȴ�  

}

void Bluetooth_send_data(uint8_t* data, uint32_t len) {
  while(data && len--){
    USART_SendData_8bit(CW_UART2, *data);
    while (USART_GetFlagStatus(CW_UART2, USART_FLAG_TXE) == RESET);  //���ͻ�����δ����ȴ�
    data++;
  }
  while (USART_GetFlagStatus(CW_UART2, USART_FLAG_TXBUSY) == SET);     //���ʹ���״̬æ��ȴ�  
}